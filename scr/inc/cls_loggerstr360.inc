<?
//============================================================//
//	STR-360ロガークラス																				//
//------------------------------------------------------------//
//	処理概要																									//
//	　地震計STR-360用																					//
//------------------------------------------------------------//
//	変更履歴																									//
//		2006/10/01	N.ARAI																			//
//		・新規作成																							//
//============================================================//
class LoggerSTR360 extends BaseLogger
{
	//変数宣言
	var $log;
	var $varsion	= "1.0";
	var $update		= "2006.10.26";
	var $memo			= "";
	var $id				= "STR360" ;

	//とりえあず今回のみロガーに閾値設定追加
	var $alert				= 0;
	var $alert_value	= "";
	var $suppl_date = "";	//最終補填日時

	//ロガー設定情報
	var $setting = array();

	var $map	= array("lgr_id"						=> "lgr_id",
		"lgr_pnt_id"				=> "lgr_pnt_id",
		"lgr_enable"				=> "lgr_enable",
		"lgr_ip"						=> "lgr_col_1",
		"lgr_tcp_port"			=> "lgr_col_2",
		"lgr_tcp_port2"			=> "lgr_col_3",
		"lgr_view_time"			=> "lgr_col_4",
		"lgr_alert_value1"	=> "lgr_col_5",
		"lgr_alert_value2"	=> "lgr_col_6",
	);


	//地震速報データTCP受信
	function ReceiveTCP($pnt_id){
		if(!$this->load_flg)	$this->LoadLoggerInfo($pnt_id);

		$ip		= "0.0.0.0";
		$port =	$this->setting["lgr_tcp_port2"];

		$socket = stream_socket_server("tcp://{$ip}:{$port}", $errno, $errstr);
		if(!$socket){
			if(is_object($this->log))	$this->log->logPrint("open-error:IP={$ip}:{$port}:ERRNO={$errno}:ERRSTR={$errstr}",REC_SOCK,__LINE__,__FILE__);
			print "open-error:IP={$ip}:{$port}:ERRNO={$errno}:ERRSTR={$errstr}\n";
		}else{
			//if(is_object($this->log))	$this->log->logPrint("open:{$ip}:{$port}",REC_SOCK,__LINE__,__FILE__);
			print "open:{$ip}:{$port}\n";

			while($conn = @stream_socket_accept($socket)){
				if(is_object($this->log))	$this->log->logPrint("connection open",REC_SOCK,__LINE__,__FILE__);
				print "connection open\n";

				$flg = 1;
				$buf = "";
				while( (!feof($conn) && $flg)){
					$tmp = fgetc($conn);

					if(chr(1)==$tmp) continue;

					$buf .= $tmp;
					if(chr(3)==$tmp){
						$bcc_chk 	= @fgetc($conn);
						$bcc_chk .= @fgetc($conn);
						$flg = 0;
					}
				}
				$bcc	= $this->BCC($buf);
				$buf .= $bcc_chk;
				if($bcc[0].$bcc[1]!=$bcc_chk){
					if(is_object($this->log))	$this->log->logPrint("BCC check error:[{$buf}]",REC_SOCK,__LINE__,__FILE__);
					print "BCC check error:[{$buf}]\n";
					$buf = "";
				}else{
					if(is_object($this->log))	$this->log->logPrint("recv:[{$buf}]",REC_SOCK,__LINE__,__FILE__);
					print "recv:[{$buf}]\n";
					fwrite($conn, chr(6));
					if(is_object($this->log))	$this->log->logPrint("send:ACK",REC_SOCK,__LINE__,__FILE__);
					print "send:ACK\n";
				}

				fclose($conn);
				if(is_object($this->log))	$this->log->logPrint("connection close",REC_SOCK,__LINE__,__FILE__);
				print "connection close\n";

				//速報データ受信時
				if(strpos($buf,"CSOKDT")){
					if(substr($buf,33,1)!="0"){
						if(is_object($this->log))	$this->log->logPrint("close",REC_SOCK,__LINE__,__FILE__);
						fclose($socket);
						print "CSOKDT [".substr($buf,32,1)."]\n";
						return $buf;
					}
				}
				//確定データ受信時
				if(strpos($buf,"CJISSD")){
					if(substr($buf,119,1)!="0"){
						if(is_object($this->log))	$this->log->logPrint("close",REC_SOCK,__LINE__,__FILE__);
						fclose($socket);
						print "CJISSD [".substr($buf,120,1)."]\n";
						return $buf;
					}
				}

			}
			if(is_object($this->log))	$this->log->logPrint("close",REC_SOCK,__LINE__,__FILE__);
			fclose($socket);
		}
	}

	//データ受信
	function ReceiveData($pnt_id,$buf){
		switch($buf){
			case strpos($buf,"CSOKDT") > 0:
				$this->ReceiveDataCSOKDT($pnt_id,$buf);
				break;
			case strpos($buf,"CJISSD") > 0:
				$this->ReceiveDataCJISSD($pnt_id,$buf);
				break;
		}
	}
	//速報データ
	function ReceiveDataCSOKDT($pnt_id,$buf){
		$args = array();
		$args["t800_pnt_id"]	= $pnt_id;
		$args["t800_sdt"]			= substr($buf,18,14);
		$args["t800_quake"]		= $this->sindo(substr($buf,32,2));
		$args["t800_quake2"]	= substr($buf,34,3);
		$args["t800_gal"]			= substr($buf,37,5);
		$args["t800_si"]			= substr($buf,43,4);

		$this->SaveQuake($args);
	}
	//確定データ
	function ReceiveDataCJISSD($pnt_id,$buf){
		$args = array();
		$args["t800_pnt_id"]	= $pnt_id;
		$args["t800_sdt"]			= substr($buf,76,14);
		$args["t800_quake"]		= $this->sindo(substr($buf,119,2));
		$args["t800_quake2"]	= substr($buf,121,3);
		$args["t800_gal"]			= substr($buf,128,5);
		$args["t800_si"]			= substr($buf,157,4);

		$this->SaveQuake($args,1);
	}
	//日本語震度表示変換
	function sindo($quake){
		$quake = trim($quake);
		$quake = str_replace("H","強",$quake);
		$quake = str_replace("L","弱",$quake);
		return trim($quake);
	}
	//地震データ保存
	function SaveQuake($args,$final=""){
		//データ整形
		$args["t800_quake2"]	= number_format($args["t800_quake2"]	/ 100, 2, ".", "");
		$args["t800_gal"]			= number_format($args["t800_gal"]			/ 10, 1, ".", "");
		$args["t800_si"]			= number_format($args["t800_si"] 			/ 10, 1, ".", "");

		$date = DateConvert2($args["t800_sdt"]);

		$trg = $date["year"]."-".$date["month"]."-".$date["day"]." ".$date["hour"].":".$date["min"].":".$date["sec"];

		$strWhere = "t800_pnt_id = '".$args["t800_pnt_id"]."' AND t800_sdt = '$trg'";
		if(GetRecordCount("t800_dat",$strWhere)>0){
			if($final){
				$strSql = "UPDATE t800_dat SET t800_quake = '".$args["t800_quake"]."',t800_quake2 = ".$args["t800_quake2"].",t800_gal = ".$args["t800_gal"].",t800_si = ".$args["t800_si"].",t800_ldt = now() WHERE {$strWhere};";
			}else{
				$strSql = "UPDATE t800_dat SET t800_quake = '".$args["t800_quake"]."',t800_quake2 = ".$args["t800_quake2"].",t800_gal = ".$args["t800_gal"].",t800_si = ".$args["t800_si"]." WHERE {$strWhere};";
			}
			print $strSql."\n";
		}else{
			if($final){
				$strSql = "INSERT INTO t800_dat VALUES('".$args["t800_pnt_id"]."','{$trg}','".$args["t800_quake"]."',".$args["t800_quake2"].",".$args["t800_si"].",".$args["t800_gal"].",now());";
			}else{
				$strSql = "INSERT INTO t800_dat VALUES('".$args["t800_pnt_id"]."','{$trg}','".$args["t800_quake"]."',".$args["t800_quake2"].",".$args["t800_si"].",".$args["t800_gal"].",'{$trg}');";
			}
			print $strSql."\n";
		}

		$dbs = new CONN;
		if($dbs->Execute($strSql)){
			return true;
		}else{
			print $dbs->getError()."\n";
			if(is_object($this->log))	$this->log->logPrint($dbs->getError(),REC_NOTE,__LINE__,__FILE__);
			return false;
		}
	}

	//地震データ取得
	function LoadData($pnt_id,$now=0){
		if(!$this->load_flg)	$this->LoadLoggerInfo($pnt_id);

		if($now){
			$args = array();
			$args["date"]		= "";
			$args["quake"]	= "";
			$args["si"]			= "";
			$args["gal"]		= "";

			$strWhere =  "t800_pnt_id = '{$pnt_id}' AND (t800_ldt IS NULL OR t800_ldt >= '".date("Y-m-d H:i:s",time()-$this->setting["lgr_view_time"]*60)."')";
			$dbs = new CONN;
			if($row=$dbs->Query("SELECT * FROM t800_dat WHERE {$strWhere} ORDER BY t800_sdt DESC LIMIT 1;")){
				if($row["t800_sdt"]!=""){
					$args["date"]		= date("m月d日 H:i:s",strtotime($row["t800_sdt"]));
				}
				$args["quake"]	= $row["t800_quake"];
				$args["si"]			= $row["t800_si"];
				$args["gal"]		= $row["t800_gal"];
			}

			//閾値判断
			$gal					= $args["gal"] + 0;
			$alert_value1 = $this->setting["lgr_alert_value1"] + 0;
			$alert_value2 = $this->setting["lgr_alert_value2"] + 0;


			if($gal >= $alert_value2){
				$this->alert				= 2;
				$this->alert_value	= "警報発生 最大加速度が".$this->setting["lgr_alert_value2"]."を超えました<br>";
				$this->alert_value	.= "震度：".$args["quake"]." 最大加速度：".$args["gal"]."gal SI値：".$args["si"]."kine";
			}else{
				if($gal >= $alert_value1){
					$this->alert				= 1;
					$this->alert_value	= "注意報発生　最大加速度が".$this->setting["lgr_alert_value1"]."を超えました<br>";
					$this->alert_value	.= "震度：".$args["quake"]." 最大加速度：".$args["gal"]."gal SI値：".$args["si"]."kine";
				}
			}
			return $args;
		}else{
			//////////////
			return true;
		}
	}
	//地震履歴補填
	function Supplement($pnt_id){
		//補填時に確定データが来ていないことが考えにくいのでサーバにて確定処理ができなかったレコードは強制的に確定処理する
		$strSql = "UPDATE t800_dat SET t800_ldt = now() WHERE t800_ldt IS NULL;";
		$dbs = new CONN;
		if(!$dbs->Execute($strSql)){
			if(is_object($this->log))	$this->log->logPrint($dbs->getError(),REC_NOTE,__LINE__,__FILE__);
		}

		if(!$this->load_flg)	$this->LoadLoggerInfo($pnt_id);
		for($i=0;$i<10;$i++){
			$recv = trim($this->SendCmd("CJISRD","001".sprintf("%04d",$i)));
			if(substr($recv,4,6)!="RJISRD") continue;
			$args = array();
			$args["t800_pnt_id"]	= $pnt_id;
			$args["t800_sdt"]			= substr($recv,21,14);
			$args["t800_quake"]		= $this->sindo(substr($recv,35,2));
			$args["t800_quake2"]	= substr($recv,37,3);
			$args["t800_gal"]			= substr($recv,44,5);
			$args["t800_si"]			= substr($recv,40,4);
			if($args["t800_quake"]==""||$args["t800_quake"]=="0") continue;
			$this->SaveQuake($args);
			//最終補填日時記録
			$this->suppl_date = time();
		}
		print "test\n";
		$recv = trim($this->SendCmd("CJISRD","0100000"));
	}


//		//通信チェック
//		function CheckConnection($pnt_id){
//			if(!$this->load_flg)	$this->LoadLoggerInfo($pnt_id);
//
//			$ip		= $this->setting["lgr_ip"];
//			$port =	$this->setting["lgr_tcp_port"];
//
//			$fp = fsockopen($ip, $port, $errno, $errstr, 10);
//
//			if(!$fp){
//			  if(is_object($this->log))	$this->log->logPrint("open-error:IP={$ip}:{$port}:ERRNO={$errno}:ERRSTR={$errstr}",REC_SOCK,__LINE__,__FILE__);
//			  print "open-error:IP={$ip}:{$port}:ERRNO={$errno}:ERRSTR={$errstr}\n";
//			}else{
//				if(is_object($this->log))	$this->log->logPrint("open:{$ip}:{$port}",REC_SOCK,__LINE__,__FILE__);
//				print "open:{$ip}:{$port}\n";
//
//				//電文送信
//				//$cmd = "0020"."CJISRD".chr(02)."001"."00000".chr(03);
//				$cmd = "0026"."CTIMST".chr(02)."20061027"."150000".chr(03);
//
//				//$cmd = "0012"."CLIFCK".chr(02).chr(03);
//				$bcc = $this->BCC($cmd);
//				$cmd = chr(01).$cmd.$bcc[0].$bcc[1];
//
//				print $cmd."\n";
//
//				fputs($fp,$cmd);
//
////				$rvalue  = fgetc($fp);
////				$rvalue  = fgetc($fp);
////				$rvalue  = fgetc($fp);
//
//				//$rvalue .= fgets($fp,450);
//				while (false !== ($char = fgetc($fp))) {
//				    //echo ord($char)."\n";
//				    echo "$char\n";
//				}
//
//				print "rvalue[".$rvalue."]";
//		    fclose($fp);
//			}
//			return $result;
//		}



	//通信チェック
	function CheckConnection($pnt_id){
		if(!$this->load_flg)	$this->LoadLoggerInfo($pnt_id);
		return $this->SendCmd("CLIFCK");
	}


	function SendCmd($cmd,$data=""){
		if(!$this->load_flg)	$this->LoadLoggerInfo($pnt_id);

		$ip		= $this->setting["lgr_ip"];
		$port =	$this->setting["lgr_tcp_port"];

		$fp = stream_socket_client("tcp://{$ip}:{$port}", $errno, $errstr, 30);
		if(!$fp){
			if(is_object($this->log))	$this->log->logPrint("open-error:IP={$ip}:{$port}:ERRNO={$errno}:ERRSTR={$errstr}",REC_SOCK,__LINE__,__FILE__);
		}else{
			if(is_object($this->log))	$this->log->logPrint("open:{$ip}:{$port}",REC_SOCK,__LINE__,__FILE__);
			//電文送信
			$cmd = $this->CreateCmd($cmd,$data);
			print "send:[{$cmd}]\n";
			if(is_object($this->log))	$this->log->logPrint("send:{$cmd}",REC_SOCK,__LINE__,__FILE__);
			fwrite($fp, $cmd);

			$flg = 1;
			$buf = "";
			while( (!feof($fp) && $flg)){
				$tmp = fgetc($fp);
				if(chr(6)==$tmp) continue;
				if(chr(1)==$tmp) continue;
				$buf .= $tmp;
				if(chr(3)==$tmp){
					$bcc_chk 	= @fgetc($fp);
					$bcc_chk .= @fgetc($fp);
					$flg = 0;
				}
			}

			$bcc	= $this->BCC($buf);
			$buf .= $bcc_chk;
			if($bcc[0].$bcc[1]!=$bcc_chk){
				if(is_object($this->log))	$this->log->logPrint("BCC check error:[{$buf}]",REC_SOCK,__LINE__,__FILE__);
				print "BCC check error:[{$buf}]\n";
				$buf = "";
			}else{
				if(is_object($this->log))	$this->log->logPrint("recv:[{$buf}]",REC_SOCK,__LINE__,__FILE__);
				print "recv:[{$buf}]\n";
			}
			if(is_object($this->log))	$this->log->logPrint("send:ACK",REC_SOCK,__LINE__,__FILE__);
			fwrite($fp, chr(6));
			if(is_object($this->log))	$this->log->logPrint("close",REC_SOCK,__LINE__,__FILE__);
			fclose($fp);
		}
		return $buf;
	}


	function CreateCmd($cmd,$data=""){
		$strCmd = $cmd.chr(02).$data.chr(03);
		$length = sprintf("%04d",strlen($strCmd)+4);
		$strCmd = $length.$strCmd;
		$bcc = $this->BCC($strCmd);
		return chr(01).$strCmd.$bcc[0].$bcc[1];
	}
	function BCC($str){
		for($i = 0; $i<strLen($str); $i++) {
			if($i == 0) {
				$tmp = substr($str, $i, 1);
			} else {
				$tmp = $tmp ^ substr($str, $i, 1);
			}
		}
		$tmp = sprintf("%b", ord($tmp));
		$ret = Array();
		if(strlen($tmp) <=4){
			$ret[0] = "0";
		} else {
			$ret[0] = strtoupper(dechex(bindec(substr($tmp, 0, strlen($tmp) - 4))));
		}
		$ret[1] = strtoupper(dechex(bindec(substr($tmp, strlen($tmp) - 4, 4))));
		return $ret;
	}

	//時刻補正
	function SetDate($pnt_id){
		if(!$this->load_flg)	$this->LoadLoggerInfo($pnt_id);
		$recv =  $this->SendCmd("CTIMST",date("YmdHis"));
		if(strpos(" ".$recv,"RTIMST")>0){
			$result = "時刻補正が完了しました[".$this->id."]";
		}else{
			$result = "時刻補正に失敗しました[".$this->id."]";
		}
		return $result;
	}


	function GetSumData($pnt_id,$sys_no,$curTime,&$args){
		return true;
	}
	function GetData($pnt_id,$sys_no,$curTime,&$args){
		return true;
	}

	//局のロガー設定を保存
	function SaveLoggerInfo($args){
		$AryTmp = array();

		foreach($this->map as $key => $val){
			if($val=="lgr_enable"){
				$AryTmp[] = "$val = ".($args[$key]=="" ? 0 : 1)."";
			}else{
				if(strpos(" ".$key,"lgr_alert_value")>0){
					$AryTmp[] = "$val = '".number_format($args[$key]+0, 2, ".", "")."'";
				}else{
					$AryTmp[] = "$val = '".$args[$key]."'";
				}
			}

		}

		$strSql = "UPDATE t_lgrdat SET ".join(",",$AryTmp)." WHERE lgr_id = '".$args["lgr_id"]."' AND lgr_pnt_id = '".$args["lgr_pnt_id"]."';";

		if(is_object($this->log))	$this->log->logPrint($strSql,REC_NOTE,__LINE__,__FILE__);

		$dbs = new CONN;
		if($dbs->Execute($strSql)){
			$this->load_flg = 0;
			return true;
		}else{
			if(is_object($this->log))	$this->log->logPrint($dbs->getError(),REC_NOTE,__LINE__,__FILE__);
			return false;
		}
	}

}
?>