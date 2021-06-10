<?
//============================================================//
//		共通関数																								//
//------------------------------------------------------------//
//	変更履歴																									//
//		2006/09/25	N.ARAI																			//
//		・新規作成																							//
//============================================================//
define("COMMON_INC", 1);
if(!defined("CONST_INC"))		include("const.inc");
if(!defined("COMMON_INC"))	include(ROOT_INC."common.inc");

function __autoload($className){
	include_once ROOT_INC.strtolower("cls_".$className.".inc");
}

function ChangeKIJI($buf,$start=1){
	$AryLine	= explode("\n",$buf);
	$buf			= "";
	foreach($AryLine as $line){
		$AryTmp = explode(",",$line);
		for($l=$start;$l<count($AryTmp);$l++){
			if(strpos(" ".$AryTmp[$l],"/")>0){
				$kiji_flg = "/";
			}elseif(strpos(" ".$AryTmp[$l],":")>0){
				$kiji_flg = ":";
			}else{
				$kiji_flg = "";
			}
			if($kiji_flg!=""){
				$t = explode($kiji_flg,$AryTmp[$l]);
				if(is_numeric($t[0]))	$AryTmp[$l] = "'".$AryTmp[$l]."";
			}
		}
		$buf .= join(",",$AryTmp)."\n";
	}
	return $buf;
}

function GetDirectionCode($d,$ws=""){

	if(is_numeric($ws)){
		if($ws==0) return 0;
	}else{
		return "";
	}

	if($d > 360) $d = $d - 360;

	if(0 <= $d && $d < 11.25){
		return 1;
	}elseif(11.25  <= $d && $d < 33.75) {
		return 2;
	}elseif(33.75	 <= $d && $d < 56.25) {
		return 3;
	}elseif(56.25  <= $d && $d < 78.75) {
		return 4;
	}elseif(78.75	 <= $d && $d < 101.25) {
		return 5;;
	}elseif(101.25 <= $d && $d < 123.75) {
		return 6;
	}elseif(123.75 <= $d && $d < 146.25) {
		return 7;
	}elseif(146.25 <= $d && $d < 168.75) {
		return 8;
	}elseif(168.75 <= $d && $d < 191.25) {
		return 9;
	}elseif(191.25 <= $d && $d < 213.75) {
		return 10;
	}elseif(213.75 <= $d && $d < 236.25) {
		return 11;
	}elseif(236.25 <= $d && $d < 258.75) {
		return 12;
	}elseif(258.75 <= $d && $d < 281.25) {
		return 13;
	}elseif(281.25 <= $d && $d < 303.75) {
		return 14;
	}elseif(303.75 <= $d && $d < 326.25) {
		return 15;
	}elseif(326.25 <= $d && $d < 348.75) {
		return 16;
	}elseif(348.75 <= $d){
		return 1;
	}else{
		return 0;
	}

}

function LastDate($y,$m){
	$m++;
	if($m==13){
		$y++;
		$m = 1;
	}
	$t = strtotime("{$y}/{$m}/1 00:01")-86400;
	return date("d",$t);
}

function CheckStart($tmg,$curTime){
	if($tmg<1) return false;

	if($curTime>time()) return false;

	//起動タイミングチェック
	switch($tmg){
		case "1":
			break;
		case "10":
			if(substr(date("i",$curTime),-1)!="0") return false;
			break;
		case "60":
			if(date("i",$curTime)!="00") return false;
			break;
		case "1440":
			if(date("Hi",$curTime)!="0000") return false;
			break;
		case "9999":
			if(date("dHi",$curTime)!=10000) return false;
			break;
		default:
			return false;
	}
	return true;
}

//-------------------------------------------------------------------
//処理 : 日付計算処理
//
//引数 : $trg		= yyyymmddhhiiss
//       $typ 	= 減算する項目(y,m,d,h,i,s)
//       $value = 減算する値
//戻値 : array(['日時']=>yyyymmddhhiiss,['unixtime']=UnixTime)
//-------------------------------------------------------------------
function DateAdd($trg,$typ,$value){
	$year 	= substr($trg, 0,		4);
	$month 	= substr($trg, 4,		2);
	$day 		= substr($trg, 6,		2);
	$hour 	= substr($trg, 8,		2);
	$minute = substr($trg, 10,	2);
	$second = substr($trg, 12,	2);

	switch($typ){
		case 'y':
			$year		+= $value;
			break;
		case 'm':
			$month	+= $value;
			break;
		case 'd':
			$day		+= $value;
			break;
		case 'h':
			$hour		+= $value;
			break;
		case 'i':
			$minute	+= $value;
			break;
		case 's':
			$second	+= $value;
			break;
	}
	$curTime	= mktime($hour, $minute, $second, $month,	$day,	$year);
	$dt				= DATE("YmdHis",$curTime);
	$arr			= array('日時'=>$dt,'unixtime'=>$curTime);
	return $arr;
}

//システム時刻をIWS仕様にあわせる
function DateConvert($curTime){
	$args = array();

	if(date("Hi",$curTime)==0){
		$curTime = $curTime - 60;
		$args["hour"]	= "24";
		$args["min"]	=	"00";
	}else{
		$args["hour"]	= sprintf("%02d",date("H",$curTime));
		$args["min"]	=	sprintf("%02d",date("i",$curTime));
	}
	$args["year"]		= date("Y",$curTime);
	$args["month"]	= sprintf("%02d",date("m",$curTime));
	$args["day"]		= sprintf("%02d",date("d",$curTime));
	$args["date"]		= $args["year"].$args["month"].$args["day"].$args["hour"].$args["min"];

	return $args;
}

//連結日付文字列をIWS仕様にあわせる
function DateConvert2($trg){
	$args["year"]  	= substr($trg,0,4);
	$args["month"] 	= substr($trg,4,2);
	$args["day"]   	= substr($trg,6,2);
	$args["hour"]  	= substr($trg,8,2);
	$args["min"]   	= substr($trg,10,2);
	$args["sec"]   	= substr($trg,12,2);
	$args["date"]		= $args["year"].$args["month"].$args["day"].$args["hour"].$args["min"];
	$args["date2"]	= $args["year"].$args["month"].$args["day"].$args["hour"].$args["min"].$args["sec"];
	return $args;
}


//連結日付文字列を通常UnixTimeに変換
function DateConvert3($trg){
	$y  = substr($trg,0,4);
	$m	= substr($trg,4,2);
	$d  = substr($trg,6,2);
	$h  = substr($trg,8,2);
	$i  = substr($trg,10,2);

	if($h=="24"){
		$time = strtotime("$y/$m/$d 00:00") + 86400;
	}else{
		$time = strtotime("$y/$m/$d $h:$i");
	}
	return $time;
}

////	//SEQの最大値を取得
////	function GetNewCode($tbl){
////		$dbs = new CONN;
////		$row=$dbs->Query("select nextval('$tbl');");
////		return $row['nextval'];
////	}
////

//指定したテーブルの件数
function GetRecordCount($tbl,$where){
	$dbs = new CONN;
	if(!$row=$dbs->Query("select count(*) from $tbl".($where==""?"":" where $where").";")){
		return 0;
	}else{
		return $row["count"];
	}
}


//JavaScript対応
function check($arr) {
	$ret = $arr;
	foreach($arr as $key => $val) {

		$v0="";
		$k0="";
		if(is_array($val)){
			foreach($val as $k0 => $v0) {
				$v1="";
				$k1="";
				if(is_array($v0)){
					foreach($v0 as $k1 => $v1) {
						$ret[$key][$k0][$k1] = str_replace('"', '”', $v1);
						$ret[$key][$k0][$k1] = str_replace('\'', '’', $ret[$key][$k0][$k1]);
						$ret[$key][$k0][$k1] = str_replace(',', '，', $ret[$key][$k0][$k1]);
						$ret[$key][$k0][$k1] = htmlspecialchars($ret[$key][$k0][$k1]);
						//$ret[$key][$k0][$k1] = escapeshellcmd($ret[$key][$k0][$k1]);
					}
				} else {
					$ret[$key][$k0] = str_replace('"', '”', $v0);
					$ret[$key][$k0] = str_replace('\'', '’', $ret[$key][$k0]);
					$ret[$key][$k0] = str_replace(',', '，', $ret[$key][$k0]);
					$ret[$key][$k0] = htmlspecialchars($ret[$key][$k0]);
					//$ret[$key][$k0] = escapeshellcmd($ret[$key][$k0]);
				}
			}
		} else {
			$ret[$key] = str_replace('"', '”', $val);
			$ret[$key] = str_replace('\'', '’', $ret[$key]);
			$ret[$key] = str_replace(',', '，', $ret[$key]);
			$ret[$key] = htmlspecialchars($ret[$key]);
			//$ret[$key] = escapeshellcmd($ret[$key]);
		}
	}
	return $ret;
}

//指定した年月から月末が正しいかどうかを判断
function checkEndofTheMonth($arr)
{
	$arr = $arr == "" ? date("Ymd") : $_GET['trg'];

	$year = intval(substr($arr, 0, 4));
	$month = intval(substr($arr, 4, 2));
	$day = intval(substr($arr, 6, 2));

	$endOfTheMonth = 0;
	//月末判定
	if($month != 2)
	{
		switch($month)
		{
			case 4:
			case 6:
			case 9:
			case 11:
				$endOfTheMonth = 30;
				break;
			default:
				$endOfTheMonth = 31;
				break;
		}
	}
	else
	{
		//閏年かどうか確認し2月の月末を判定
		if($year % 4 == 0)
		{
			if($year % 100 == 0)
			{
				if($year % 400 == 0)
				{
					$endOfTheMonth = 29;
				}
				else
				{
					$endOfTheMonth = 28;
				}
			}
			else
			{
				$endOfTheMonth = 29;
			}
		}
		else
		{
			$endOfTheMonth = 28;
		}
	}

	//引数から割り出した日にちから正しいかどうかを判定する
	if($endOfTheMonth >= $day)
	{
		return $arr;
	}
	else
	{
		//日付を修正した文字列を返す
		$changeArr = strval($year) .sprintf("%02d",$month) .sprintf("%02d",$endOfTheMonth);
		return $changeArr;
	}
}


////
////
////	//メールアドレス妥当性チェック
////	function mailcheck($ma){
////		$ok =	 ereg("^[0-9,A-Z,a-z][0-9,a-z,A-Z,_,\.,-]+@
////									[0-9,A-Z,a-z][0-9,a-z,A-Z,_,\.,-]+\.
////									(af|al|dz|as|ad|ao|ai|aq|ag|ar|am|aw|ac|au|at|az|
////									bh|bd|bb|by|bj|bm|bt|bo|ba|bw|br|io|bn|bg|bf|bi|kh|
////									cm|ca|cv|cf|td|gg|je|cl|cn|cx|cc|co|km|cg|cd|ck|cr|
////									ci|hr|cu|cy|cz|dk|dj|dm|do|tp|ec|eg|sv|gq|er|ee|et|
////									fk|fo|fj|fi|fr|gf|pf|tf|fx|ga|gm|ge|de|gh|gi|gd|gp|gu|
////									gt|gn|gw|gy|ht|hm|hn|hk|hu|is|in|id|ir|iq|ie|im|il|it|
////									jm|jo|kz|ke|ki|kp|kr|kw|kg|la|lv|lb|ls|lr|ly|li|lt|lu|
////									mo|mk|mg|mw|my|mv|ml|mt|mh|mq|mr|mu|yt|mx|fm|md|mc|mn|
////									ms|ma|mz|mm|na|nr|np|nl|an|nc|nz|ni|ne|ng|nu|nf|mp|no|
////									om|pk|pw|pa|pg|py|pe|ph|pn|pl|pt|pr|qa|re|ro|ru|rw|kn|
////									lc|vc|ws|sm|st|sa|sn|sc|sl|sg|sk|si|sb|so|za|gs|es|lk|
////									sh|pm|sd|sr|sj|sz|se|ch|sy|tw|tj|tz|th|bs|ky|tg|tk|to|
////									tt|tn|tr|tm|tc|tv|ug|ua|ae|uk|us|um|uy|uz|vu|va|ve|vn|
////									vg|vi|wf|eh|ye|yu|zm|zw|
////									com|net|org|gov|edu|int|mil|biz|info|name|pro|jp)
////									$",$ma);
////		return $ok;
////	}
////
////	//ログ書き出し
////	function KANRI_LOG($msg,$level,$l,$f){
////		session_cache_limiter("public");
////		session_start();
////
////		$file = LOG_DIR.date("Ym")."_.log";
////
////		$log = date("Y-m-d(D) H:i:s")."  [".$_SERVER['REMOTE_ADDR'].":".$_SESSION["id"]."]  ";
////		switch($level){
////			case LOG_NOTE:
////				$log .= " Note :";
////				break;
////			case LOG_ERROR:
////				$log .= " ERROR:";
////				break;
////			case LOG_ALERT:
////				$log .= " ALERT:";
////				break;
////			default:
////				$log .= "      :";
////		}
////		$log .= str_replace("\n", "", $msg);
////		$log .= "  ({$f}:{$l})";
////
////		$fp=fopen($file, "a");
////		fputs($fp, "{$log}\n");
////		fclose($fp);
////		@chmod($file, 0777);
////
////		//エラー通知
////		switch($level){
////			case LOG_ERROR:
////				break;
////			case LOG_ALERT:
////				break;
////			default:
////		}
////	}
////

//ユーザチェック
function user_check($level){
	if(!isset($_SESSION)){
		session_cache_limiter("public");
		session_start();
	}

	$id		= $_SESSION["id"];
	$pwd	=	$_SESSION["pwd"];

	if($_SESSION["u_level"]!=99&&$_SESSION["id"]!=""&&$_SESSION["pwd"]!=""){
		$user_level = $_SESSION["u_level"];
	}else{
		$dbs 		= new CONN;
		$row		= $dbs->Query("SELECT * FROM t_admmst WHERE adm_id = '$id';");
		if($row["adm_id"]){
			if(trim($row["adm_pwd"])==trim(crypt($pwd,$row["adm_pwd"]))){
				if($level>$_SESSION["u_level"]){
					return false;
				}else{
					$user_level = $row["adm_level"];
				}
			}else{
				return false;
			}
		}else{
			return false;
		}
	}

	$AryTmp = array();
	switch($user_level){
		case USER:
			$AryTmp["USER"]								= 1;
			$AryTmp["USER_OP_UNDEF"]			= 1;
			$AryTmp["USER_ADMIN_UNDEF"]		= 1;
			$AryTmp["USER_SYSADM_UNDEF"]	= 1;
			$AryTmp["USER_ROOT_UNDEF"]		= 1;
			break;
		case USER_OP:
			$AryTmp["USER"] 							= 1;
			$AryTmp["USER_OP"] 						= 1;
			$AryTmp["USER_ADMIN_UNDEF"]		= 1;
			$AryTmp["USER_SYSADM_UNDEF"]	= 1;
			$AryTmp["USER_ROOT_UNDEF"]		= 1;
			break;
		case USER_ADMIN:
			$AryTmp["USER"] 							= 1;
			$AryTmp["USER_OP"] 						= 1;
			$AryTmp["USER_ADMIN"]					= 1;
			$AryTmp["USER_SYSADM_UNDEF"]	= 1;
			$AryTmp["USER_ROOT_UNDEF"]		= 1;
			break;
		case USER_SYSADM:
			$AryTmp["USER"] 							= 1;
			$AryTmp["USER_OP"]						= 1;
			$AryTmp["USER_ADMIN"] 				= 1;
			$AryTmp["USER_SYSADM"] 				= 1;
			$AryTmp["USER_ROOT_UNDEF"]		= 1;
			break;
		case USER_ROOT:
			$AryTmp["USER"] 							= 1;
			$AryTmp["USER_OP"]						= 1;
			$AryTmp["USER_ADMIN"] 				= 1;
			$AryTmp["USER_SYSADM"] 				= 1;
			$AryTmp["USER_ROOT"]					= 1;
			break;
		default:
			return false;
	}
	$_SESSION["u_level"] = $user_level;
	return $AryTmp;
}

//定期データ取得処理起動	[2006/12/29]
function StartCron($pnt_id,$curTime,$sys_no){
	switch($sys_no){
		case "000":
			$id = "Present";
			break;
		case "100":
			$id = "Minutely";
			break;
		case "200":
			$id = "Daily";
			break;
		case "300":
			$id = "Monthly";
			break;
		case "400":
			$id = "Yearly";
			break;
		default:
			return false;
	}

	if(is_numeric($pnt_id)){
		//局毎データ取得処理

		$log = new Recorder("","C ".strtoupper($id),$pnt_id);

		$pnt = new Points($log);
		if(!$Point=$pnt->LoadPointInfo($pnt_id)){
			$log->logPrint("LoadPointInfo({$pnt_id}) Error",REC_NOTE,__LINE__,__FILE__);
		}

		//起動タイミングチェック
		if(!CheckStart($Point["pnt_tmg_".$sys_no],$curTime))	exit();

		$plock = new PLOCK($log);//プロセス監視
		$plock->LockStart("{$pnt_id}_{$sys_no}");

		//データ取得スタート
		$date = DateConvert($curTime);
		$log->logPrint("Start {$id} Cron pnt_id => {$pnt_id} Target Date =>".$date["date"],REC_NOTE,__LINE__,__FILE__);

		//実行時間待機
		sleep($Point["pnt_wait_".$sys_no]);
		$log->logPrint("Wait Start ".$Point["pnt_wait_".$sys_no]."sec",REC_NOTE,__LINE__,__FILE__);

		//データ取得フィールド定義
		$data = new Data($log);
		if(!$rec=$data->GetDataField($sys_no)){
			$log->logPrint("GetDataField({$sys_no}) Error",REC_NOTE,__LINE__,__FILE__);
		}

		//リトライカウント初期化
		$retry 			= 0;
		$retry_flg	= true;
		//局毎ロガー取得
		$logger = $pnt->GetLoggers($pnt_id);
		while(1){
			for($i=0;$i<count($logger);$i++){
				if($logger[$i]["lgr_enable"]=="1" && $logger[$i]["result"]!="1"){
					//クラス名取得
					$clsTmp		= $logger[$i]["class"];
					//クラス定義
					$cls[$i]	= new $clsTmp($log);
					//データ取得
					if($cls[$i]->GetData($pnt_id,$sys_no,$curTime,$rec)) $logger[$i]["result"] = 1;
				}else{
					$logger[$i]["result"] = 1;
				}
			}

			//データ取得結果チェック
			$cnt = 0;
			for($i=0;$i<count($logger);$i++){
				if($logger[$i]["result"]=="1") $cnt++;
			}

			if($cnt==count($logger)) break;

			//リトライカウント０の場合は無視
			if($Point["pnt_retry_cnt_".$sys_no]==0) break;
			//リトライ時間待機＆リトライ回数チェック
			$retry++;
			if($Point["pnt_retry_cnt_".$sys_no] >= $retry){
				sleep($Point["pnt_retry_wait_".$sys_no]);
				$log->logPrint("Retry Wait ".$Point["pnt_retry_wait_".$sys_no]."sec",REC_NOTE,__LINE__,__FILE__);
			}else{
				$retry_flg	= false;
				$log->logPrint("Retry Count Over",REC_NOTE,__LINE__,__FILE__);
				break;
			}
		}

		////天気別取得
		//if(array_key_exists("weather",$rec)) $rec["weather"] = BaseLogger::GetWeather($pnt_id);

		//取得データ保存
		if($data->SaveData($pnt_id,$sys_no,$curTime,$rec,"",1)){
			$log->logPrint("{$id} Cron Data Save Succeeded",REC_DEBUG,__LINE__,__FILE__);
		}else{
			$log->logPrint("{$id} Cron Data Save Failed",REC_NOTE,__LINE__,__FILE__);
		}

		//現在データ
		if($sys_no=="000"&&$retry_flg){
			//閾値判断
			$alt = new Alert($log);
			$alt->CheckAlert($pnt_id,$curTime,$rec);

			//XMLデータ出力
			$strCmd = PATH_PHP." ".ROOT_DIR."cron/present_data.php {$pnt_id} '".date("Y/m/d H:i",$curTime)."' > /dev/null &";
			$log->logPrint($strCmd,REC_DEBUG,__LINE__,__FILE__);
			system($strCmd);
		}

		/////集計データ保存/////
		$SkipSave = "";
		switch($sys_no){
			case "200":
			case "300":
			case "400":
				$log->logPrint("Start {$id} Cron SumData pnt_id => {$pnt_id} Target Date =>".$date["date"],REC_DEBUG,__LINE__,__FILE__);
				$rec = $data->GetSumDataField($sys_no);
				for($i=0;$i<count($logger);$i++){
					//クラス名取得
					if(!is_object($cls[$i])){
						//クラス再定義（ここで失敗している場合は処理が正常にできていない可能性あり）
						$clsTmp = $logger[$i]["class"];
						$cls[$i] = new $clsTmp($log);
					}
					//データ取得
					$cls[$i]->GetSumData($pnt_id,$sys_no,$curTime,$rec);
					$SkipSave = $cls[$i]->SkipSave[$sys_no];
				}
				if($SkipSave===true) break;

				if($data->SaveSumData($pnt_id,$sys_no,$curTime,$rec)){
					$log->logPrint("{$id} SumData Data Save Succeeded",REC_DEBUG,__LINE__,__FILE__);
				}else{
					$log->logPrint("{$id} SumData Data Save Failed",REC_NOTE,__LINE__,__FILE__);
				}
				break;
		}
		$log->logPrint("End {$id} Cron pnt_id => {$pnt_id} Target Date =>".$date["date"],REC_NOTE,__LINE__,__FILE__);

		$plock->LockEnd();
		exit();
	}

	//局毎データ取得起動処理
	$log = new Recorder("cron");
	$pnt = new Points($log);
	foreach($pnt->GetPoints("",1) as $val){
		$strCmd = PATH_PHP." ".ROOT_DIR."cron/weather_cron.php {$sys_no} ".$val["pnt_id"]." > /dev/null &";
		$log->logPrint($strCmd,REC_DEBUG,__LINE__,__FILE__);
		system($strCmd);
	}
}


//再計算処理
function ReCalculation($pnt_id,$year,$month,$log="",$day="",$Recal1minFlg=true,$RecalDisregard=false,$AryColSys=""){
	if(is_object($log)) $log->logPrint("ReCalculation Start pnt => {$pnt_id} Target => {$year}/{$month}",REC_NOTE,__LINE__,__FILE__);

	//局情報取得
	$pnt		= new Points($log);
	$Point	= $pnt->LoadPointInfo($pnt_id);
	//その局が使用しているロガーから再計算取得できる帳票を取得
	$col		= $pnt->GetColSys($pnt_id);

	//////////////////////////////
	if(is_array($AryColSys)){
		$col = $AryColSys;
	}
	//////////////////////////////

	//局毎ロガー取得
	$logger = $pnt->GetLoggers($pnt_id);
	if(is_object($log))	$log->logPrint("ReCalculation ColSys => ".join("/",$col),			REC_DEBUG,__LINE__,__FILE__);
	if(is_object($log))	$log->logPrint("ReCalculation Logger => ".print_r($logger,TRUE),REC_DEBUG,__LINE__,__FILE__);

	if($day==""){
		//時間設定
		$curTime 	= strtotime("{$year}/{$month}/01 00:00");
		//エンド時間
		$y = $year;
		$m = $month;
		$m++;
		if($m>12){
			$y++;
			$m="01";
		}
		$curTime2	= strtotime("{$y}/{$m}/01 00:00");
	}else{
		$curTime 	= strtotime("{$year}/{$month}/{$day} 00:00");
		$curTime2	= strtotime("{$year}/{$month}/{$day} 00:00")+86400;
	}
	if(is_object($log))	$log->logPrint("ReCalculation Start => ".date("Y/m/d H:i",$curTime)." End => ".date("Y/m/d H:i",$curTime2),REC_DEBUG,__LINE__,__FILE__);

	$dbs = new CONN;

	//データ削除
	for($sys_no=200;$sys_no<=400;$sys_no+=100){
		if(in_array($sys_no,$col))	continue;
		$tbl		= "t{$sys_no}_dat_".$year;
		$trg 		= $year.$month.$day;
		$strSql = "DELETE FROM {$tbl} WHERE t{$sys_no}_pnt_id ='{$pnt_id}' AND t{$sys_no}_date LIKE '{$trg}%' AND t{$sys_no}_code IS NULL;";
		if(is_object($log))	$log->logPrint("ReCalculation ".$strSql,REC_DEBUG,__LINE__,__FILE__);
		$dbs->Execute($strSql);
	}

	//データ再計算開始
	$add_time = 600;
	$curTime += $add_time;
	while($curTime <= $curTime2){

		for($sys_no=200;$sys_no<=400;$sys_no+=100){
			//ロガー取得できない帳票のみ処理する
			if(in_array($sys_no,$col))	continue;
			//起動タイミングチェック
			if(!CheckStart($Point["pnt_tmg_".$sys_no],$curTime))	continue;

			if(is_object($log))	$log->logPrint("ReCalculation Target Start GetData    => [{$sys_no}] ".date("Y/m/d H:i",$curTime),REC_DEBUG,__LINE__,__FILE__);

			//データ取得フィールド定義
			$data = new Data($log);
			$rec	= $data->GetDataField($sys_no);

			for($i=0;$i<count($logger);$i++){
				if($logger[$i]["lgr_enable"]=="1"){
					//クラス名取得
					$clsTmp		= $logger[$i]["class"];
					//クラス定義
					$cls[$i]	= new $clsTmp($log);
					///////////////////////
					$cls[$i]->Recal1minFlg 		= $Recal1minFlg;
					$cls[$i]->RecalDisregard	= $RecalDisregard;
					///////////////////////
					//データ取得
					$cls[$i]->GetData($pnt_id,$sys_no,$curTime,$rec);
				}else{
					$logger[$i]["result"] = 1;
				}
			}
			//取得データ保存
			if($data->SaveData($pnt_id,$sys_no,$curTime,$rec)){
				//$log->logPrint("{$id} Cron Data Save Succeeded",REC_NOTE,__LINE__,__FILE__);
			}else{
				//$log->logPrint("{$id} Cron Data Save Failed",REC_NOTE,__LINE__,__FILE__);
			}

			if($sys_no=="400") break;	//年報の集計処理は最後に行う

			/////集計データ保存/////
			if(!CheckStart($Point["pnt_tmg_".$sys_no],$curTime)) continue;

			switch($sys_no){
				case "200":
					if($year.$month==date("Ym")){
						if(($curTime + 3600) < time()){
							if(date("Hi",$curTime)!="0000")	continue 2;
						}
					}else{
						if(date("Hi",$curTime)!="0000") continue 2;
					}
					break;
				case "300":
					if($year.$month==date("Ym")){
						if(($curTime + 86400) < time())	continue 2;
					}else{
						if(date("dHi",$curTime)!="010000") continue 2;
					}
					break;
			}
			if(is_object($log))	$log->logPrint("ReCalculation Target Start GetSumData => [{$sys_no}] ".date("Y/m/d H:i",$curTime),REC_DEBUG,__LINE__,__FILE__);

			$rec = $data->GetSumDataField($sys_no);
			for($i=0;$i<count($logger);$i++){
				//クラス名取得
				$clsTmp = $logger[$i]["class"];
				if(!is_object($cls[$i])) $cls[$i] = new $clsTmp($log);	//クラス再定義
				///////////////////////
				$cls[$i]->Recal1minFlg 		= $Recal1minFlg;
				$cls[$i]->RecalDisregard	= $RecalDisregard;
				///////////////////////
				//データ取得
				$cls[$i]->GetSumData($pnt_id,$sys_no,$curTime,$rec);
			}
			if($data->SaveSumData($pnt_id,$sys_no,$curTime,$rec)){
				//$log->logPrint("{$id} SumData Data Save Succeeded",REC_NOTE,__LINE__,__FILE__);
			}else{
				//$log->logPrint("{$id} SumData Data Save Failed",REC_NOTE,__LINE__,__FILE__);
			}
			/////集計データ保存/////
		}
		$curTime += $add_time;
	}

	//処理日付を再セット
	if(is_numeric($day)){
		$lastDay	= LastDate($year,$month);
		$tmpTime 	= strtotime("{$year}/{$month}/{$lastDay} 00:00")+86400;
		if($tmpTime>time()){
			$curTime = strtotime(date("Y/m/d")." 00:00");
		}else{
			$curTime = $tmpTime;
		}
		$sys_no = "300";
		if(is_object($log))	$log->logPrint("ReCalculation Target Start GetSumData => [{$sys_no}] ".date("Y/m/d H:i",$curTime),REC_DEBUG,__LINE__,__FILE__);

		$rec = $data->GetSumDataField($sys_no);
		for($i=0;$i<count($logger);$i++){
			//クラス名取得
			$clsTmp = $logger[$i]["class"];
			if(!is_object($cls[$i])) $cls[$i] = new $clsTmp($log);	//クラス再定義
			///////////////////////
			$cls[$i]->Recal1minFlg 		= $Recal1minFlg;
			$cls[$i]->RecalDisregard	= $RecalDisregard;
			///////////////////////
			//データ取得
			$cls[$i]->GetSumData($pnt_id,$sys_no,$curTime,$rec);
		}
		if($data->SaveSumData($pnt_id,$sys_no,$curTime,$rec)){
			//$log->logPrint("{$id} SumData Data Save Succeeded",REC_NOTE,__LINE__,__FILE__);
		}else{
			//$log->logPrint("{$id} SumData Data Save Failed",REC_NOTE,__LINE__,__FILE__);
		}
	}

	//年報のみ当月の前の月までのデータにて再計算処理を毎回おこなう
	//処理日付を再セット
	if($year==date("Y")){
		if(date("m")==1){
			$flg = 0;
		}else{
			$curTime = strtotime("{$year}/".date("m")."/01 00:00");
			$flg = 1;
		}
	}else{
		$curTime = strtotime(($year+1)."/01/01 00:00");
		$flg = 1;
	}
	if($flg){
		$sys_no = "400";
		if(is_object($log))	$log->logPrint("ReCalculation Target Start GetSumData => [{$sys_no}] ".date("Y/m/d H:i",$curTime),REC_DEBUG,__LINE__,__FILE__);

		$rec = $data->GetSumDataField($sys_no);
		for($i=0;$i<count($logger);$i++){
			//クラス名取得
			$clsTmp = $logger[$i]["class"];
			if(!is_object($cls[$i])) $cls[$i] = new $clsTmp($log);	//クラス再定義
			///////////////////////
			$cls[$i]->Recal1minFlg 		= $Recal1minFlg;
			$cls[$i]->RecalDisregard	= $RecalDisregard;
			///////////////////////
			//データ取得
			$cls[$i]->GetSumData($pnt_id,$sys_no,$curTime,$rec);
		}
		if($data->SaveSumData($pnt_id,$sys_no,$curTime,$rec)){
			//$log->logPrint("{$id} SumData Data Save Succeeded",REC_NOTE,__LINE__,__FILE__);
		}else{
			//$log->logPrint("{$id} SumData Data Save Failed",REC_NOTE,__LINE__,__FILE__);
		}
	}
	if(is_object($log)) $log->logPrint("ReCalculation End   pnt => {$pnt_id} Target => {$year}/{$month}",REC_NOTE,__LINE__,__FILE__);
	return "再集計が完了しました";
}

//再計算処理２/////////////////////////////（基本的に再計算処理をそのままコピー当日のみ再計算実施処理）
function ReCalculation2($pnt_id,$curTime,$log=""){
	//局情報取得
	$pnt		= new Points($log);
	$Point	= $pnt->LoadPointInfo($pnt_id);
	//その局が使用しているロガーから再計算取得できる帳票を取得
	$col		= $pnt->GetColSys($pnt_id);
	//局毎ロガー取得
	$logger = $pnt->GetLoggers($pnt_id);

	$year		= date("Y",$curTime);
	$month	= date("m",$curTime);
	$day		= date("d",$curTime);
	$clsTmp	= $logger[0]["class"];	//クラス名取得
	$cls[0]	= new $clsTmp;					//クラス定義
	if(!$cls[0]->ReCal) return false;

	if(is_object($log)) $log->logPrint("ReCalculation Start pnt => {$pnt_id} Target => {$year}/{$month}",REC_NOTE,__LINE__,__FILE__);

	if(is_object($log))	$log->logPrint("ReCalculation ColSys => ".join("/",$col),			REC_DEBUG,__LINE__,__FILE__);
	if(is_object($log))	$log->logPrint("ReCalculation Logger => ".print_r($logger,TRUE),REC_DEBUG,__LINE__,__FILE__);

	//時間設定
	//$curTime 	= strtotime("{$year}/{$month}/01 00:00");
	$curTime 	= strtotime("{$year}/{$month}/{$day} 00:00");
	//エンド時間
//		$y = $year;
//		$m = $month;
//		$m++;
//		if($m>12){
//			$y++;
//			$m="01";
//		}
//		$curTime2	= strtotime("{$y}/{$m}/01 00:00");
	$curTime2	= $curTime + 86400;
	if(is_object($log))	$log->logPrint("ReCalculation Start => ".date("Y/m/d H:i",$curTime)." End => ".date("Y/m/d H:i",$curTime2),REC_DEBUG,__LINE__,__FILE__);

	$dbs = new CONN;

	//データ削除
	for($sys_no=200;$sys_no<=400;$sys_no+=100){
		if(in_array($sys_no,$col))	continue;
		$tbl		= "t{$sys_no}_dat_".$year;
		//$trg 		= $year.$month;
		$trg 		= $year.$month.$day;
		if($sys_no=="400")	$trg = $year.$month;
		$strSql = "DELETE FROM {$tbl} WHERE t{$sys_no}_pnt_id ='{$pnt_id}' AND t{$sys_no}_date LIKE '{$trg}%' AND t{$sys_no}_code IS NULL;";
		if(is_object($log))	$log->logPrint("ReCalculation ".$strSql,REC_DEBUG,__LINE__,__FILE__);
		$dbs->Execute($strSql);
	}

	//データ再計算開始
	$add_time = 600;
	$curTime += $add_time;
	while($curTime <= $curTime2){

		for($sys_no=200;$sys_no<=400;$sys_no+=100){
			//ロガー取得できない帳票のみ処理する
			if(in_array($sys_no,$col))	continue;
			//起動タイミングチェック
			if($sys_no=="400"){
				if(!CheckStart($Point["pnt_tmg_300"],$curTime))				continue;
				if(date("Ym",$curTime-60)==date("Ym")){
					continue;
				}
			}else{
				if(!CheckStart($Point["pnt_tmg_".$sys_no],$curTime))	continue;
			}

			if(is_object($log))	$log->logPrint("ReCalculation Target Start GetData    => [{$sys_no}] ".date("Y/m/d H:i",$curTime),REC_DEBUG,__LINE__,__FILE__);

			//データ取得フィールド定義
			$data = new Data($log);
			$rec	= $data->GetDataField($sys_no);

			for($i=0;$i<count($logger);$i++){
				if($logger[$i]["lgr_enable"]=="1"){
					//クラス名取得
					$clsTmp		= $logger[$i]["class"];
					//クラス定義
					$cls[$i]	= new $clsTmp($log);
					//データ取得
					$cls[$i]->GetData($pnt_id,$sys_no,$curTime,$rec);
				}else{
					$logger[$i]["result"] = 1;
				}
			}
			//取得データ保存
			if($data->SaveData($pnt_id,$sys_no,$curTime,$rec)){
				//$log->logPrint("{$id} Cron Data Save Succeeded",REC_NOTE,__LINE__,__FILE__);
			}else{
				//$log->logPrint("{$id} Cron Data Save Failed",REC_NOTE,__LINE__,__FILE__);
			}

			if($sys_no=="400") break;	//年報の集計処理は最後に行う

			/////集計データ保存/////
			if(!CheckStart($Point["pnt_tmg_".$sys_no],$curTime)) continue;

			switch($sys_no){
				case "200":
					if($year.$month==date("Ym")){
						if(($curTime + 3600) < time()){
							if(date("Hi",$curTime)!="0000")	continue 2;
						}
					}else{
						if(date("Hi",$curTime)!="0000") continue 2;
					}
					break;
				case "300":
					if($year.$month==date("Ym")){
						if(($curTime + 86400) < time())	continue 2;
					}else{
						//if(date("dHi",$curTime)!="010000") continue 2;
					}
					break;
			}
			if(is_object($log))	$log->logPrint("ReCalculation Target Start GetSumData => [{$sys_no}] ".date("Y/m/d H:i",$curTime),REC_DEBUG,__LINE__,__FILE__);

			$rec = $data->GetSumDataField($sys_no);
			for($i=0;$i<count($logger);$i++){
				//クラス名取得
				$clsTmp = $logger[$i]["class"];
				if(!is_object($cls[$i])) $cls[$i] = new $clsTmp($log);	//クラス再定義
				//データ取得
				$cls[$i]->GetSumData($pnt_id,$sys_no,$curTime,$rec);
			}
			if($data->SaveSumData($pnt_id,$sys_no,$curTime,$rec)){
				//$log->logPrint("{$id} SumData Data Save Succeeded",REC_NOTE,__LINE__,__FILE__);
			}else{
				//$log->logPrint("{$id} SumData Data Save Failed",REC_NOTE,__LINE__,__FILE__);
			}
			/////集計データ保存/////
		}
		$curTime += $add_time;
	}

	//年報のみ当月の前の月までのデータにて再計算処理を毎回おこなう
	//処理日付を再セット
	if($year==date("Y")){
		if(date("m")==1){
			$flg = 0;
		}else{
			$curTime = strtotime("{$year}/".date("m")."/01 00:00");
			$flg = 1;
		}
	}else{
		$curTime = strtotime(($year+1)."/01/01 00:00");
		$flg = 1;
	}
	if($flg){
		$sys_no = "400";
		if(is_object($log))	$log->logPrint("ReCalculation Target Start GetSumData => [{$sys_no}] ".date("Y/m/d H:i",$curTime),REC_DEBUG,__LINE__,__FILE__);

		$rec = $data->GetSumDataField($sys_no);
		for($i=0;$i<count($logger);$i++){
			//クラス名取得
			$clsTmp = $logger[$i]["class"];
			if(!is_object($cls[$i])) $cls[$i] = new $clsTmp($log);	//クラス再定義
			//データ取得
			$cls[$i]->GetSumData($pnt_id,$sys_no,$curTime,$rec);
		}
		if($data->SaveSumData($pnt_id,$sys_no,$curTime,$rec)){
			//$log->logPrint("{$id} SumData Data Save Succeeded",REC_NOTE,__LINE__,__FILE__);
		}else{
			//$log->logPrint("{$id} SumData Data Save Failed",REC_NOTE,__LINE__,__FILE__);
		}
	}
	if(is_object($log)) $log->logPrint("ReCalculation End   pnt => {$pnt_id} Target => {$year}/{$month}",REC_NOTE,__LINE__,__FILE__);
	return "再集計が完了しました";
}
//再計算処理２/////////////////////////////

//グラフ用
function GetRuler($sys_no,&$args,$scale,$idx){
	global $GRAPH_COLOR;
	$AryTmp = explode("，",$scale);
	$filename = "../image/graph/";
	switch($AryTmp[0]){
		case "m/s":
			$filename .= "wind_";
			break;
		case "C":
			$filename .= "temp_";
			break;
		case "%":
			$filename .= "humid_";
			break;
		case "hPa":
			$filename .= "press_";
			break;
		case "m":
			$filename .= "water_";
			break;
		case "MJ":
			if($sys_no=="400"){
				$filename .= "ins_yearly_";
			}else{
				$filename .= "ins_";
			}
			break;
		case "h":
			if($sys_no=="400"){
				$filename .= "sun_yearly_";
			}else{
				$filename .= "sun_";
			}
			break;
		case "cm":
			$filename .= "snow_";
			$value = $AryTmp[count($AryTmp)-1];
			$filename .= $value."_";
			break;
		case "mm":
			$sys_no = $sys_no=="200" ? "100" : $sys_no;
			$filename .= "rain_".($sys_no)."_";
			break;
		case "mm/h":
			$filename .= "rainfall_";
			break;
	}
	if($filename!=""){
		$filename .= $GRAPH_COLOR[$idx]["id"].".png";
		if(file_exists($filename)){
			$args[$idx] = $filename;
		}else{
			return false;
		}
	}else{
		return false;
	}
}
function GetColor($img,$idx){
	global $GRAPH_COLOR;
	$r = $GRAPH_COLOR[$idx]["R"];
	$g = $GRAPH_COLOR[$idx]["G"];
	$b = $GRAPH_COLOR[$idx]["B"];
	$color = imagecolorallocate($img, $r, $g, $b);
	return $color;
}
?>
