<?
if(!defined("CONST_INC"))	include(dirname(__FILE__)."/inc/const.inc");
if(!defined("COMMON_INC"))	include(ROOT_INC."common.inc");

// 各種クラス定義
$dbs = new CONN;
$dbs_sum = new CONN;

// aws-sdk設定
require dirname(dirname(__FILE__))."/vendor/autoload.php";
use Aws\DynamoDb\Marshaler;
use Aws\Sdk;

// 処理対象局設定
$id_map = array(
	"logical-20001" => array(
		"pnt_id" => "drain",
		"logger" => "loggerN_DRAIN",
		"point" => array(
			"024" => array("type" => "water","id" => "d1"),	// 下水１
			"025" => array("type" => "water","id" => "d2"),	// 下水２
			"026" => array("type" => "water","id" => "d3"),	// 下水３
			"027" => array("type" => "water","id" => "d4"),	// 下水４
			"028" => array("type" => "water","id" => "d5"),	// 下水５
			"029" => array("type" => "water","id" => "d6"),	// 下水６
			"030" => array("type" => "water","id" => "d7"),	// 下水７
			"031" => array("type" => "water","id" => "d8"),	// 下水８
			"032" => array("type" => "water","id" => "d9")	// 下水９
		)
	),
	"logical-20002" => array(
		"pnt_id" => "suginami",
		"logger" => "loggerN_SUGINAMI",
		"point" => array(
			"007" => array("type" => "rain","id" => "s1"),	// 下井草(杉並)
			"008" => array("type" => "rain","id" => "s2"),	// 原寺分橋(杉並)
			"022" => array("type" => "water","id" => "s1"),	// 武蔵野橋(杉並)
			"023" => array("type" => "water","id" => "s2"),	// 永久橋(杉並)
			"020" => array("type" => "water","id" => "s3"),	// 久我山橋(杉並)
			"021" => array("type" => "water","id" => "s4")	// 番屋橋(杉並)
		)
	),
	"logical-20003" => array(
		"pnt_id" => "tokyo",
		"logger" => "loggerN_TOKYO",
		"point" => array(
			"006" => array("type" => "rain","id" => "t1"),	// 久我山(都)
			"018" => array("type" => "water","id" => "t1"),	// 鷺盛橋(都)
			"019" => array("type" => "water","id" => "t2")	// 佃橋(都)
		)
	),
	"logical-00002" => array("pnt_id" => "001","type" => "rain"),	// 弥生Ｃ
	"logical-00003" => array("pnt_id" => "003","type" => "rain"),	// 江古田Ｃ
	"logical-00004" => array("pnt_id" => "004","type" => "rain"),	// 南中野Ｃ
	"logical-00005" => array("pnt_id" => "005","type" => "rain"),	// 鷺宮C

	"logical-00009" => array("pnt_id" => "009","type" => "water"),	// 神善合流
	"logical-00010" => array("pnt_id" => "010","type" => "water"),	// 寿橋
	"logical-00011" => array("pnt_id" => "011","type" => "water"),	// 氷川橋
	"logical-00012" => array("pnt_id" => "012","type" => "water"),	// 末広橋
	"logical-00013" => array("pnt_id" => "014","type" => "water"),	// 双鷺橋
	"logical-00014" => array("pnt_id" => "013","type" => "water"),	// 太陽橋
	"logical-00015" => array("pnt_id" => "015","type" => "water"),	// 千歳橋
	"logical-00016" => array("pnt_id" => "016","type" => "water"),	// 妙江合流
	"logical-00017" => array("pnt_id" => "017","type" => "water"),	// 北江古田公園
	"logical-00018" => array("pnt_id" => "041","type" => "flow")	// 貯水池
);

// 各種設定値取得
$logical_id = trim($argv[3]);
$start		= trim($argv[1]);
$end		= trim($argv[2]);

$conf = $id_map[$logical_id];
if($conf["pnt_id"]==""&&$logical_id!="ALL"){
	print "logical ID Set Error\n";
	exit();
}

$t = strtotime($start);
if($t===false){
	print "Start Time Set Error\n";
	exit();
}
if(strlen($start)==10){
	$t = @strtotime($start." 00:10");
}
$start_time = $t;

if($end==""){
	$end_time = $t;
}else{
	$t = @strtotime($end);
	if($t===false){
		print "End Time Set Error\n";
		exit();
	}
	if(strlen($end_time)==10){
		$t = @strtotime($end_time." 00:00");
	}
	$end_time = $t;
}
print "Start Time => ".date("Y/m/d H:i:s",$start_time)."\n";
print "End Time   => ".date("Y/m/d H:i:s",$end_time)."\n";
print "logicalId  => ".$logical_id."\n";
print "Start OK ?\n";
$ans = fgets(STDIN);

$AryID = array();
if($logical_id=="ALL"){
	$AryID = $id_map;
}else{
	$AryID[$logical_id] = $id_map[$logical_id];
}

$wk_ym = "";
$params = [
    'TableName' => "iws-sls-minutely-store-prod",
    'Item' => $item
];

while($start_time<=$end_time){
	$date = DateConvert($start_time);
	$date2 = DateConvert($end_time);
	if($wk_ym!=$date["year"].$date["month"]){
		$ArySum = array();
		
		$strSql = "SELECT * FROM t100_dat_".$date["year"].$date["month"]." WHERE t100_date BETWEEN '".$date["date"]."' AND '".$date2["date"]."' ORDER BY t100_date;";
		if($logical_id=="logical-00018"){
			$strSql = "SELECT * FROM t000_dat_".$date["year"].$date["month"]." WHERE t000_date BETWEEN '".$date["date"]."' AND '".$date2["date"]."' ORDER BY t000_date;";
		}
		print $strSql."\n";
		$row = $dbs->Query($strSql);
		while($row){
			// 集計値レコード検索
			$trg_day = substr($row["t100_date"],0,8);
			if($logical_id=="logical-00018") $trg_day = substr($row["t000_date"],0,8);
			if(!is_array($ArySum[$trg_day])){
				$strSql = "SELECT * FROM t200_dat_".substr($trg_day,0,4)." WHERE t200_date LIKE '".$trg_day."%' AND t200_code IS NOT NULL;";
				print $strSql."\n";
				$row_sum = $dbs_sum->Query($strSql);
				while($row_sum){
					$ArySum[$trg_day][ $row_sum["t200_code"] ] = $row_sum;
					$row_sum = $dbs_sum->Next();
				}
				// aws用インスタンスを再生成（タイムアウト対策）
				$sdk = new Sdk([
				    'endpoint' => 'http://dynamodb.ap-northeast-1.amazonaws.com',
				    'region'   => 'ap-northeast-1',
				    'version'  => 'latest'
				]);
				$dynamodb = $sdk->createDynamoDb();
				$marshaler = new Marshaler();
			}
			foreach($AryID as $logical_id => $conf){
				// 全局処理の場合は貯水池を除外
				if(trim($argv[3])=="ALL"&&$logical_id=="logical-00018") continue;
				
				print "t100_date => ".$row["t100_date"]." logical_id[".$logical_id."]\n";
				
				$tmp = [];
				if(is_array($conf["point"])){
					foreach($conf["point"] as $old_id => $conf_sub){
						if($conf_sub["type"]=="rain") $tmp_sub = parseRain(array("pnt_id" => $old_id),$row);
						
						$water_type = $conf["pnt_id"];
						if($conf_sub["type"]=="water") $tmp_sub = parseWater(array("pnt_id" => $old_id),$row,$water_type);

						foreach($tmp_sub as $key => $val){
							if($key=="processing") continue;
							if($key=="logger_status") continue;
	
							if(
								$key=="water_level"||
								$key=="max__water_level"||
								$key=="max__water_level_time"||
								$key=="move_rain_10min"||
								$key=="move_rain_hour"||
								$key=="max__move_rain_10min"||
								$key=="max__move_rain_10min_time"||
								$key=="max__move_rain_hour"||
								$key=="max__move_rain_hour_time"||
								$key=="rain_day"||
								$key=="rain_rui"
							){
								$tmp[$key."_".$conf_sub["id"]] = $val;
								continue;
							}
							$tmp[$key] = $val;
						}
					}
					$tmp["loggerType"] = $conf["logger"];
				}else{
					if($conf["type"]=="rain") $tmp = parseRain($conf,$row);
					if($conf["type"]=="water") $tmp = parseWater($conf,$row);
					if($conf["type"]=="flow") $tmp = parseFlow($conf,$row);
				}
				$tmp["logicalId"] = $logical_id;

				$item = $marshaler->marshalJson(json_encode($tmp));
				// print_r($item);
				$params["Item"] = $item;
				
				try{
				    $result = $dynamodb->putItem($params);
				}catch(DynamoDbException $e){
					print $e->getMessage()."\n";
				}
				usleep(100000);
			}
			
			$row = $dbs->Next();
		}
		$wk_ym = $date["year"].$date["month"];
	}
	$start_time += 600;
}
exit();

$strSql = "SELECT * FROM t100_dat_".$date["year"].$date["month"]." WHERE t100_date BETWEEN '".$date["date"]."' AND '".$date2["date"]."' ORDER BY t100_date;";
print $strSql."\n";

$ArySum = array();

$row = $dbs->Query($strSql);
while($row){

	
	
	// 集計値レコード検索
	$trg_day = substr($row["t100_date"],0,8);
	if(!is_array($ArySum[$trg_day])){
		$strSql = "SELECT * FROM t200_dat_".substr($trg_day,0,4)." WHERE t200_date LIKE '".$trg_day."%' AND t200_code IS NOT NULL;";
		print $strSql."\n";
		$row_sum = $dbs_sum->Query($strSql);
		while($row_sum){
			$ArySum[$trg_day][ $row_sum["t200_code"] ] = $row_sum;
			$row_sum = $dbs_sum->Next();
		}
		
		$sdk = new Sdk([
		    'endpoint' => 'http://dynamodb.ap-northeast-1.amazonaws.com',
		    'region'   => 'ap-northeast-1',
		    'version'  => 'latest'
		]);
		$dynamodb = $sdk->createDynamoDb();
		$marshaler = new Marshaler();
		
	}
	
	if($conf["type"]=="rain") $tmp = parseRain($conf,$row);
	if($conf["type"]=="water") $tmp = parseWater($conf,$row);
	$tmp["logicalId"] = $logical_id;

	$item = $marshaler->marshalJson(json_encode($tmp));
	// print_r($item);
	$params = [
	    'TableName' => "iws-sls-minutely-store-prod",
	    'Item' => $item
	];
	// print $row["t100_date"]."\n";
	try {
	    $result = $dynamodb->putItem($params);
	    // logger()->notice("Added item", $item);
	} catch (DynamoDbException $e) {
		print $e->getMessage()."\n";
	    // logger()->critical("Unable to add item.\nMessage: $e->getMessage()", $params);
	}
	usleep(200000);

	$row = $dbs->Next();
}

/*

$params = [
    'TableName' => "iws-sls-minutely-store-prod",
    'Item' => $item
];

try {
    $result = $dynamodb->putItem($params);
    // logger()->notice("Added item", $item);
} catch (DynamoDbException $e) {
	print $e->getMessage()."\n";
    // logger()->critical("Unable to add item.\nMessage: $e->getMessage()", $params);
}
*/

exit();



function parseRain($conf,$row){
	global $ArySum;
	
	$pnt_id = $conf["pnt_id"];
	
	$curTIme = DateConvert3($row["t100_date"]);
	
	$tmp = array(
				// "logicalId" => $rain_map[$pnt_id],
				"targetTime" => $curTIme,
				"execType" => "Minutely",
				"execMode" => "Automatic",
				"loggerType" => "loggerRCL80",
				"max__move_rain_10min_time" => null,
				"processing" => array(),
				"rain_rui" => null,
				"rain_day" => null,
				"move_rain_10min" => null, 
				"max__move_rain_hour" => null,
				"max__move_rain_hour_time" => null,
				"max__move_rain_10min" => null,
				"move_rain_hour" => null 
				);
	$tmp["move_rain_10min"] = is_numeric($row["move_rain_10min_".$pnt_id]) ? (float) $row["move_rain_10min_".$pnt_id] : null;
	$tmp["max__move_rain_10min"] = $tmp["move_rain_10min"];
	$tmp["move_rain_hour"] = is_numeric($row["move_rain_hour_".$pnt_id]) ? (float) $row["move_rain_hour_".$pnt_id] : null;
	$tmp["rain_day"] = is_numeric($row["rain_day_".$pnt_id]) ? (float) $row["rain_day_".$pnt_id] : null;
	$tmp["rain_rui"] = is_numeric($row["rain_rui_".$pnt_id]) ? (float) $row["rain_rui_".$pnt_id] : null;	// わざとnull


	$trg_day = substr($row["t100_date"],0,8);
	// print "t100_date => ".$row["t100_date"]."\n";
	// print $trg_day.str_replace(":","",$ArySum[$trg_day]["55"]["max_rain_10min_".$pnt_id])."\n";
	$max__move_rain_10min_time = DateConvert3($trg_day.str_replace(":","",$ArySum[$trg_day]["55"]["max_rain_10min_".$pnt_id]));
	if(substr(date("i",$max__move_rain_10min_time),-1)=="0"){
		$check_time = $max__move_rain_10min_time;
	}else{
		$check_time = $max__move_rain_10min_time+(10-substr(date("i",$max__move_rain_10min_time),-1))*60;
	}
	
	// print date("Y/m/d H:i:s",$check_time)." ".date("Y/m/d H:i:s",$curTIme)." 10min\n";
	if($curTIme==$check_time){
		$tmp["max__move_rain_10min"] = (float) $ArySum[$trg_day]["50"]["max_rain_10min_".$pnt_id];
		
		$t = DateConvert($max__move_rain_10min_time);
		// $tmp["max__move_rain_10min_time"] = date("Y-m-d H:i",$max__move_rain_10min_time);
		// $tmp["max__move_rain_10min_time"] = $t["year"]."-".$t["month"]."-".$t["day"]." ".$t["hour"].":".$t["min"];
		$tmp["max__move_rain_10min_time"] = date("Y-m-d H:i",$max__move_rain_10min_time);
		// print_r($tmp);
	}
	$max__move_rain_hour_time = DateConvert3($trg_day.str_replace(":","",$ArySum[$trg_day]["55"]["move_rain_hour_".$pnt_id]));
	
	
	// $check_time = $max__move_rain_hour_time+(10-substr(date("i",$max__move_rain_hour_time),-1))*60;
	if(substr(date("i",$max__move_rain_hour_time),-1)=="0"){
		$check_time = $max__move_rain_hour_time;
	}else{
		$check_time = $max__move_rain_hour_time+(10-substr(date("i",$max__move_rain_hour_time),-1))*60;
	}

	// print date("Y/m/d H:i:s",$check_time)." ".date("Y/m/d H:i:s",$curTIme)." hour\n";
	if($curTIme==$check_time){
		$tmp["max__move_rain_hour"] = (float) $ArySum[$trg_day]["50"]["move_rain_hour_".$pnt_id];
		$tmp["max__move_rain_hour_time"] = date("Y-m-d H:i",$max__move_rain_hour_time);
	}
	return $tmp;
}

function parseWater($conf,$row,$type="water"){
	global $ArySum;
	if($type=="drain"){
		$fld_id = "drain_1min";
	}else{
		$fld_id = "water_1min";
	}
	
	$pnt_id = $conf["pnt_id"];
	
	$curTIme = DateConvert3($row["t100_date"]);

	$tmp = array(
				// "logicalId" => $rain_map[$pnt_id],
				"targetTime" => $curTIme,
				"execType" => "Minutely",
				"execMode" => "Automatic",
				"loggerType" => "loggerCK4100",
				"processing" => array(),
				"logger_status" => 0,
				"water_level" => null,
				"max__water_level" => null,
				"max__water_level_time" => null
				);
	$tmp["water_level"] = is_numeric($row[$fld_id."_".$pnt_id]) ? (float) $row[$fld_id."_".$pnt_id] : null;
	$tmp["max__water_level"] = $tmp["water_level"];

	$trg_day = substr($row["t100_date"],0,8);
	// print "t100_date => ".$row["t100_date"]."\n";
	$max__water_level_time = DateConvert3($trg_day.str_replace(":","",$ArySum[$trg_day]["55"][$fld_id."_".$pnt_id]));
	if(substr(date("i",$max__water_level_time),-1)=="0"){
		$check_time = $max__water_level_time;
	}else{
		$check_time = $max__water_level_time+(10-substr(date("i",$max__water_level_time),-1))*60;
	}
	// print date("Y/m/d H:i:s",$check_time)." ".date("Y/m/d H:i:s",$curTIme)." 10min\n";
	if($curTIme==$check_time){
		$tmp["max__water_level"] = (float) $ArySum[$trg_day]["50"][$fld_id."_".$pnt_id];
		$t = DateConvert($max__water_level_time);
		$tmp["max__water_level_time"] = date("Y-m-d H:i",$max__water_level_time);
	}
	return $tmp;
}

function parseFlow($conf,$row){
	global $ArySum;
	
	$fld_id = "water_1min";

	$pnt_id = $conf["pnt_id"];
	
	$curTIme = DateConvert3($row["t000_date"]);

	$tmp = array(
				// "logicalId" => $rain_map[$pnt_id],
				"targetTime" => $curTIme,
				"execType" => "Minutely",
				"execMode" => "Automatic",
				"loggerType" => "loggerCK4100F",
				"processing" => array(),
				"alert_status" => 0,
				"level_status" => 0,
				"logger_status" => 0,
				"water_flow" => null,
				"water_level" => null,
				"water_level_max_10min" => null,
				"water_level_max_10min_time" => null
				);

	$tmp["water_flow"] = is_numeric($row["water_flow_".$pnt_id]) ? (float) $row["water_flow_".$pnt_id] : null;
	$tmp["water_level"] = is_numeric($row["water_1min_".$pnt_id]) ? (float) $row["water_1min_".$pnt_id] : null;
	$tmp["water_level_max_10min"] = $tmp["water_level"];
	return $tmp;
}
















$cls = new CONN;

// print "SELECT * FROM t100_dat_202002 WHERE t100_pnt_id = '{$pnt_id}'";
$row = $cls->Query("SELECT * FROM t100_dat_202002;");
print_r($row);

exit();


if($argv[2]=="now") $argv[2] = date("Y/m/d H:i");

if(!$curTime=strtotime($argv[2])){
	print "Set Date Error\n";
	exit();
}

$pnt_id =	$argv[1];
if(!is_numeric($pnt_id)){
	print "pnt_id Not Found\n";
	exit();
}
print "[".date("Y/m/d H:i",$curTime)."] [{$pnt_id}]\n";

//傾向グラフ用定義
$AryCmp = array("pnt_comp_1" => "avg_ws",
	"pnt_comp_2" => "max_ws",
	"pnt_comp_3" => "temp",
	"pnt_comp_4" => "humid1",
	"pnt_comp_5" => "humid2",
	"pnt_comp_6" => "press1",
	"pnt_comp_7" => "press2",
	"pnt_comp_8" => "water"
);

//$log = new Recorder("present","",$pnt_id);
$pnt		= new Points($log);
$Point	= $pnt->LoadPointInfo($pnt_id);

$fld_000 = Fields::GetFields("000");

$weather_flg = 0;
foreach($pnt->GetWeatherPoints() as $val){
	if($val["pnt_id"]==$pnt_id){
		$weather_flg = 1;
		break;
	}
}


if($weather_flg){
	//傾向グラフ用時間幅とスケール取得

	$AryTmg			= array();
	$AryCmpData = array();
	foreach($AryCmp as $key => $fld_id){
		if($Point[$key]==0) continue;

		//項目ごとの取得タイミング設定取得
		$AryCmpData[$fld_id]["tmg"] = $Point[$key];

		//n分前データ取得用に問い合わせる時刻を格納
		for($i=1;$i<4;$i++){
			$AryTmg[$AryCmpData[$fld_id]["tmg"] * $i]	= $AryCmpData[$fld_id]["tmg"] * $i;
		}

		$time = $Point[$key] * 3;
		switch($time){
			case $time < 60;
				$AryCmpData[$fld_id]["tmg_value"] = $time."分前";
				break;
			case $time < 1440;
				$time /= 60;
				$AryCmpData[$fld_id]["tmg_value"] = $time."時間前";
				break;
			default:
				$time /= 1440;
				$AryCmpData[$fld_id]["tmg_value"] = $time."日前";
				break;
		}
		//スケール設定
		$AryCmpData[$fld_id]["fld_scale"] = $fld_000[$fld_id]["fld_scale"];
	}

}


//現在データ取得
$data			= new Data($log);
$AryData	= array();
$AryData	= $data->LoadAryData($pnt_id,"000",$curTime);
//すべての取得タイミングデータを一括取得
if(is_array($AryTmg)){
	foreach($AryTmg as $tmg){
		//$AryData = array_merge($AryData, $data->LoadAryData($pnt_id,"000",$curTime - $tmg * 60));
		$AryData += $data->LoadAryData($pnt_id,"000",$curTime - $tmg * 60);
	}
}


$AryPresent = array();

//現在データ出力
$date = DateConvert($curTime);
foreach($AryData[ $date["date"] ] as $fld_id => $val){
	$AryPresent["now"][$fld_id] = $val;
}

//グラフ用データ出力
if(is_array($AryCmpData)){
	foreach($AryCmpData as $fld_id => $val){
		//比較スケール設定
		$scale = explode("，",$val["fld_scale"]);

		$AryTmp = array();
		$AryPresent["graph"][$fld_id."_graph_tmg"] = $val["tmg_value"];

		for($i=0;$i<4;$i++){
			$date = DateConvert($curTime - $val["tmg"] * $i * 60);
			$value = $AryData[ $date["date"] ][$fld_id];

			//n分前との傾向比較用
			if($i<2){
				if($i==0){
					$tmp_value = $value;
				}
				if($i==1){
					if($tmp_value > $value){
						$AryPresent["comp"][$fld_id."_graph_comp"] = "down";
					}elseif($tmp_value < $value){
						$AryPresent["comp"][$fld_id."_graph_comp"] = "up";
					}else{
						$AryPresent["comp"][$fld_id."_graph_comp"] = "eq";
					}
				}
			}

			//特殊（暫定傾向グラフ対応
			if($tmp_value > $value){
				$AryPresent["graph"][$fld_id."_graph_".$i] = "2";
			}elseif($tmp_value < $value){
				$AryPresent["graph"][$fld_id."_graph_".$i] = "4";
			}else{
				$AryPresent["graph"][$fld_id."_graph_".$i] = "3";
			}
		}

	}
}

if($weather_flg){
	//10分積算データ算出
	$date			= DateConvert($curTime);
	$sum			= 0;
	$cnt			=	0;
	if(substr($date["min"],-1)=="0"){
		$start	= 1;
		$total	= 7;
	}else{
		$start	= 0;
		$total	= 6;
	}
	$Ajust = substr($date["min"],-1) * 60;
	if(!is_array($AryData[ $date["date"] ])){
		print "loaded [".$date["date"]."]\n";
		$AryData = $data->LoadAryData($pnt_id,"000",$curTime);
	}
	$AryPresent["tab"]["rain_10min_value_{$cnt}"]	= $AryData[ $date["date"] ]["rain_10min"];
	$AryPresent["tab"]["rain_10min_time_{$cnt}"]	= "現在";
	$sum += (float) $AryPresent["tab"]["rain_10min_value_{$cnt}"];
	for($i=$start;$i<$total;$i++){
		$cnt++;
		$date = DateConvert($curTime - $i * 600 - $Ajust);
		$trg	= $date["year"].$date["month"].$date["day"];
		if(!is_array($AryData_100[$trg])){
			$AryData_100[ $trg ] = $data->LoadAryData($pnt_id,"100",$curTime - $i * 600 - $Ajust);
			print "{$trg} loaded!!\n";
		}
		$trg_time = $date["hour"].":".$date["min"];
		$AryPresent["tab"]["rain_10min_value_{$cnt}"]	= $AryData_100[ $trg ][ $trg_time ]["rain_10min"];
		$AryPresent["tab"]["rain_10min_time_{$cnt}"]	= $trg_time;
		$sum +=  (float) $AryPresent["tab"]["rain_10min_value_{$cnt}"];
	}
	$AryPresent["tab"]["rain_10min_value_sum"]	= number_format($sum,1, ".", "");
	$AryPresent["tab"]["rain_10min_time_sum"]		= "積算値";

	//時間積算データ算出
	$date			= DateConvert($curTime);
	$sum			= 0;
	$cnt			=	0;
	if($date["min"]=="00"){
		$start	= 1;
		$total	= 7;
	}else{
		$start	= 0;
		$total	= 6;
	}
	$Ajust = $date["min"]*60;
	if(!is_array($AryData[ $date["date"] ])){
		print "loaded [".$date["date"]."]\n";
		$AryData = $data->LoadAryData($pnt_id,"000",$curTime);
	}
	$AryPresent["tab"]["rain_hour_value_{$cnt}"]	= $AryData[ $date["date"] ]["rain_hour"];
	$AryPresent["tab"]["rain_hour_time_{$cnt}"]		= "現在";
	$sum += (float) $AryPresent["tab"]["rain_hour_value_{$cnt}"];

	for($i=$start;$i<$total;$i++){
		$cnt++;
		$date = DateConvert($curTime - $i * 3600 - $Ajust);
		$trg	= $date["year"].$date["month"].$date["day"];
		if(!is_array($AryData_100[$trg])){
			$AryData_100[ $trg ] = $data->LoadAryData($pnt_id,"100",$curTime - $i * 3600 - $Ajust);
			print "{$trg} loaded!!\n";
		}
		$trg_time = $date["hour"].":00";
		$AryPresent["tab"]["rain_hour_value_{$cnt}"]	= $AryData_100[ $trg ][ $trg_time ]["rain_hour"];
		$AryPresent["tab"]["rain_hour_time_{$cnt}"]		= $trg_time;
		$sum +=  (float) $AryPresent["tab"]["rain_hour_value_{$cnt}"];
	}
	$AryPresent["tab"]["rain_hour_value_sum"]	= number_format($sum,1, ".", "");
	$AryPresent["tab"]["rain_hour_time_sum"]	= "積算値";


	//日積算データ算出
	$date			= DateConvert($curTime);
	$sum			= 0;
	$cnt			=	0;
	$start		= 1;
	$total		= 7;
	if(!is_array($AryData[ $date["date"] ])){
		print "loaded\n";
		$AryData = $data->LoadAryData($pnt_id,"000",$curTime);
	}
	$AryPresent["tab"]["rain_day_value_{$cnt}"]	= $AryData[ $date["date"] ]["rain_day"];
	$AryPresent["tab"]["rain_day_time_{$cnt}"]	= "今日";
	$sum += (float) $AryPresent["tab"]["rain_day_value_{$cnt}"];

	//時報データ取得
	for($i=$start;$i<$total;$i++){
		$cnt++;
		$date = DateConvert($curTime - $i * 86400);
		$trg	= $date["year"].$date["month"];
		if(!is_array($AryData_300[$trg])){
			$AryData_300[ $trg ] = $data->LoadAryData($pnt_id,"300",$curTime - $i * 86400);
		}
		$trg_time = $date["day"];
		$AryPresent["tab"]["rain_day_value_{$cnt}"]	= $AryData_300[ $trg ][ $trg_time ]["rain_day"];
		$AryPresent["tab"]["rain_day_time_{$cnt}"]	= $date["month"]."/".$date["day"];
		$sum +=  (float) $AryPresent["tab"]["rain_day_value_{$cnt}"];
	}
	$AryPresent["tab"]["rain_day_value_sum"]	= number_format($sum,1, ".", "");
	$AryPresent["tab"]["rain_day_time_sum"]	= "積算値";
}


//警報情報取得
$AryAlert = Alert::GetAlert($pnt_id,$curTime);
foreach($AryAlert["tab"] as $key => $val)	$AryPresent["tab"][$key] = $val;

$t		= $Point["pnt_alert_time"];
$name = $Point["pnt_alert_time"];

$tmp = array();
$tmp["pnt_id"]					= $pnt_id;
$tmp["pnt_name"]				= $Point["pnt_name"];
$tmp["pnt_alert_time"]	=	$Point["pnt_alert_time"];
$tmp["date"]						= date("YmdHi",$curTime);
$tmp["alert_date"]			= $AryAlert["alert_date"];
$tmp["total_level"]			= $AryAlert["total_level"];
$tmp["alert_level"]			=	$AryAlert["alert_level"];
$tmp["warning_level"]		= $AryAlert["warning_level"];

$week = array("日","月","火","水","木","金","土");
if(trim($AryPresent["now"]["weather"])==""){
	$tmp["date_value"]	=	date("Y年m月d日",$curTime)." (".$week[ date("w",$curTime) ].") ".date("H:i",$curTime);
}else{
	$tmp["date_value"]	=	date("Y年m月d日",$curTime)." (".$week[ date("w",$curTime) ].") ".date("H:i",$curTime)."  天気：".$AryPresent["now"]["weather"];
}

$AryPresent["info"] = $tmp;

//ＸＭＬデータ出力
$dom = new DOMDocument("1.0","UTF-8");
$dom->formatOutput = true;

$root = $dom->createElement('iws');
$root = $dom->appendChild($root);

$node = $dom->createElement('weatherData');
foreach($AryPresent["info"] as $key => $val){
	$strName	= mb_convert_encoding( $key,"UTF-8","euc-jp" );
	$strValue	=	mb_convert_encoding( $val,"UTF-8","euc-jp" );
	$node->setAttribute( $strName , $strValue );
}
$node = $root->appendChild($node);

foreach($AryPresent["now"] as $key => $val){
	$node2 = $dom->createElement('dataset');

	$strName	= mb_convert_encoding( "fld_id","UTF-8","euc-jp" );
	$strValue	=	mb_convert_encoding( $key,		"UTF-8","euc-jp" );
	$node2->setAttribute( $strName , $strValue );
	$strName	= mb_convert_encoding( "value",	"UTF-8","euc-jp" );
	$strValue	=	mb_convert_encoding( $val,		"UTF-8","euc-jp" );
	$node2->setAttribute( $strName , $strValue );

	$node2 = $node->appendChild($node2);
}

if(is_array($AryPresent["tab"])){
	$node = $dom->createElement('weatherTab');
	$node = $root->appendChild($node);
	foreach($AryPresent["tab"] as $key => $val){
		$node2 = $dom->createElement('dataset');

		if(is_array($val)){
			$strName	= mb_convert_encoding( "fld_id","UTF-8","euc-jp" );
			$strValue	=	mb_convert_encoding( $key,		"UTF-8","euc-jp" );
			$node2->setAttribute( $strName , $strValue );
			foreach($val as $war_key => $war_val){
				$strName	= mb_convert_encoding( $war_key,"UTF-8","euc-jp" );
				$strValue	=	mb_convert_encoding( $war_val,"UTF-8","euc-jp" );
				$node2->setAttribute( $strName , $strValue );
			}
		}else{
			$strName	= mb_convert_encoding( "fld_id","UTF-8","euc-jp" );
			$strValue	=	mb_convert_encoding( $key,		"UTF-8","euc-jp" );
			$node2->setAttribute( $strName , $strValue );
			$strName	= mb_convert_encoding( "value",	"UTF-8","euc-jp" );
			$strValue	=	mb_convert_encoding( $val,		"UTF-8","euc-jp" );
			$node2->setAttribute( $strName , $strValue );
		}

		$node2 = $node->appendChild($node2);
	}
}

if(is_array($AryPresent["graph"])){
	$node = $dom->createElement('weatherGraph');
	$node = $root->appendChild($node);
	foreach($AryPresent["graph"] as $key => $val){
		$node2 = $dom->createElement('dataset');

		$strName	= mb_convert_encoding( "fld_id","UTF-8","euc-jp" );
		$strValue	=	mb_convert_encoding( $key,		"UTF-8","euc-jp" );
		$node2->setAttribute( $strName , $strValue );
		$strName	= mb_convert_encoding( "value",	"UTF-8","euc-jp" );
		$strValue	=	mb_convert_encoding( $val,		"UTF-8","euc-jp" );
		$node2->setAttribute( $strName , $strValue );

		$node2 = $node->appendChild($node2);
	}
}

if(is_array($AryPresent["comp"])){
	$node = $dom->createElement('weatherComp');
	$node = $root->appendChild($node);
	foreach($AryPresent["comp"] as $key => $val){
		$node2 = $dom->createElement('dataset');

		$strName	= mb_convert_encoding( "fld_id","UTF-8","euc-jp" );
		$strValue	=	mb_convert_encoding( $key,		"UTF-8","euc-jp" );
		$node2->setAttribute( $strName , $strValue );
		$strName	= mb_convert_encoding( "value",	"UTF-8","euc-jp" );
		$strValue	=	mb_convert_encoding( $val,		"UTF-8","euc-jp" );
		$node2->setAttribute( $strName , $strValue );

		$node2 = $node->appendChild($node2);
	}
}
$dom->save(XML_DIR."present_{$pnt_id}.xml");
chmod(XML_DIR."present_{$pnt_id}.xml",0777);

//自動出動データ出力処理
$ftp_file = trim($Point["pnt_ftp"]);
if($ftp_file!=""){
	//複数タイプ対応（とりあえず暫定）
	if(strpos(" ".$ftp_file,"DATA")>0||strpos(" ".$ftp_file,"00000000")>0){
		$flg = 1;
	}else{
		$flg = 0;
	}

	$ftp_file = FTP_DIR.$ftp_file;
	//日付設定
	$date = DateConvert($curTime);
	//データ再ロード
	$AryTmp = $data->LoadData($pnt_id,"000",$curTime);
	$AryNow = $AryTmp[ $date["date"] ];
	//FTP用配列
	$AryFtp["date"] = $date["year"].$date["month"].$date["day"];
	$AryFtp["time"] = $date["hour"].":".$date["min"];
	$AryFtp["avg_ws"]			=	"////";
	$AryFtp["avg_wd"]			=	"//";
	$AryFtp["max_ws"]			=	"////";
	$AryFtp["max_wd"] 		=	"//";
	$AryFtp["temp"]				=	"/////";
	$AryFtp["humid1"]			=	"////";
	$AryFtp["humid2"]			=	"////";
	$AryFtp["press1"]			=	"//////";
	$AryFtp["press2"]			=	"//////";
	$AryFtp["rain_10min"]	=	"/////";
	$AryFtp["rain_hour"]	=	"/////";
	$AryFtp["rain_day"]		=	"/////";
	if(!$flg)	$AryFtp["dummy"]			=	"";

	$cnt = 0;
	foreach($AryFtp as $key =>$val){
		if($key=="date"||$key=="time") continue;

		$value = trim($AryNow[$key]);
		if(!is_numeric($value))	continue;

		$cnt++;

		$len = strlen($val);

		$minus = "";
		if(strpos(" ".$value,"-")>0){
			//マイナスデータ時
			$value = str_replace("-","",$value);
			$minus = "-";
			$len--;
		}
		if($key!="avg_wd"&&$key!="max_wd"){
			$value = number_format($value,	1, ".", "");
		}
		while($len > strlen($value)){
			$value = "0".$value;
		}
		$AryFtp[$key] = $minus.$value;
	}
	if($cnt>0){
		//データ書き出し
		$buf = join(",",$AryFtp)."\r";
		if($flg){
			$ftp_lock  = str_replace(".CSV","",$ftp_file);
			$ftp_lock  = str_replace("DATA0","GENZAI0",$ftp_lock);
			$ftp_lock	 = str_replace("DATA1","GENZAI1",$ftp_lock);
			$ftp_lock	 = str_replace("DATA2","GENZAI2",$ftp_lock);
			$ftp_lock  = str_replace("00000000","GENZAI",$ftp_lock);
			$ftp_lock .= ".FLG";
		}else{
			$ftp_lock = str_replace(".dat",".FLG",$ftp_file);
			$ftp_lock = str_replace(".DAT",".FLG",$ftp_file);
		}
		print $ftp_file."\n";
		print $ftp_lock."\n";
		if(!file_exists($ftp_lock))	exec("touch $ftp_lock");
		exec("echo \"{$buf}\" > {$ftp_file}");
		exec("rm -f $ftp_lock");
	}
	//天気データ書きだし
	if(strpos(" ".$ftp_file,"kisyo")>0){
		$buf = $AryNow["weather"];
		$tenki_file = str_replace("kisyo","tenki",$ftp_file);
		$tenki_lock = str_replace(".DAT",".FLG",$tenki_file);
		print $tenki_file."\n";
		print $tenki_lock."\n";
		if(!file_exists($tenki_lock))	exec("touch $tenki_lock");
		exec("echo \"{$buf}\" > {$tenki_file}");
		exec("rm -f $tenki_lock");
	}

}
?>