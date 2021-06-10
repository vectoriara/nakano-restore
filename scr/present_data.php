<?php
if(!defined("CONST_INC"))	include(dirname(__FILE__)."/inc/const.inc");
if(!defined("COMMON_INC"))	include(ROOT_INC."common.inc");



$pnt_id = trim($argv[1]);


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