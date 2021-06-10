<?
	class LoggerCX1000FTP extends BaseLogger{
		//変数宣言
		var $setting		= array();						//ロガー設定情報格納用配列
		var $load_flg 	= false;							//ロガー設定情報読込みフラグ
		var $varsion		= "1.0";
		var $update			= "2012.12.19";
		var $memo				= "にかほ市専用クラスから改変し田村市";
		var $id					= "CX1000FTP";
		var $type				= "1";														//ロガータイプ　1 => 気象ロガー	 2 => 雨量ロガー
		var $map				= array("lgr_id"				=> "lgr_id",
														"lgr_pnt_id"		=> "lgr_pnt_id",
														"lgr_enable"		=> "lgr_enable",
														"lgr_time"			=> "lgr_time",
														"lgr_ftp_ip"		=> "lgr_col_1",
														"lgr_ftp_id"		=> "lgr_col_2",
														"lgr_ftp_pwd"		=> "lgr_col_3",
														"lgr_ftp_dir"		=> "lgr_col_4"
														);
		var $EditSys	= array();												//編集可能なデータ帳票
		var $ColSys		= array("100","200","300","400");	//手動収集可能なデータ帳票
		var $DelSys		= array("100","200","300","400");	//削除可能なデータ帳票

		var $flg_edit_rainrui				= true;//累計雨量編集
		var $flg_edit_weather				= false;//天気編集
		var $flg_edit_warning				= false;//気象庁編集
		var $flg_edit_alert					= true;//閾値編集

		//帳票毎でこのロガーが出力できる項目を定義
		var $OutField = array(
													"100" => array(	"avg_ws",
																					"avg_wd",
																					"max_ws",
																					"max_wd",
																					"temp",
																					"humid1",
																					"humid2",
																					"press1",
																					"rain_10min",
																					"rain_hour",
																					"rain_day",
																					"water_level",
																					"max_water_level",
																					"min_water_level",
																					"radiation",
																					"snow_level",
																					"snow_hour",
																					"press2",
																					"sun_day",
																					"sun_month"
																				),
													"200" => array(	"avg_ws",
																					"avg_wd",
																					"max_ws",
																					"max_wd",
																					"temp",
																					"humid1",
																					"humid2",
																					"press1",
																					"max_rain_10min",
																					"rain_hour",
																					"rain_day",
																					"water_level",
																					"max_water_level",
																					"min_water_level",
																					"radiation",
																					"snow_level",
																					"snow_hour",
																					"press2",
																					"sun_day",
																					"sun_month"
																				),
													"300" => array(	"avg_ws",
																					"avg_wd",
																					"max_ws",
																					"max_wd",
																					"avg_temp",
																					"max_temp",
																					"min_temp",
																					"avg_humid1",
																					"max_humid1",
																					"min_humid1",
																					"humid2",
																					"press1",
																					"avg_press1",
																					"max_press1",
																					"min_press1",
																					"max_rain_10min",
																					"max_rain_hour",
																					"rain_day",
																					"avg_water_level",
																					"max_water_level",
																					"min_water_level",
																					"radiation",
																					"snow_level",
																					"snow_day",
																					"press2",
																					"sun_day",
																					"sun_month"
																				),
													"400" => array("avg_ws",
																			    "avg_wd",
																			    "max_ws",
																			    "max_wd",
																			    "avg_temp",
																			    "max_temp",
																			    "min_temp",
																			    "avg_humid1",
																			    "max_humid1",
																			    "min_humid1",
																			    "humid2",
																			    "avg_humid2",
																			    "max_humid2",
																			    "min_humid2",
																			    "press1",
																			    "avg_press1",
																			    "max_press1",
																			    "min_press1",
																			    "max_rain_10min",
																			    "max_rain_hour",
																			    "max_rain_day",
																			    "rain_month",
																					"avg_water_level",
																					"max_water_level",
																					"min_water_level",
																					"radiation",
																					"snow_level",
																					"snow_month",
																					"max_snow_day",
																					"press2",
																					"sun_day",
																					"sun_month"
																				),
													"000" => array("avg_ws",
																					"avg_wd",
																					"max_ws",
																					"max_wd",
																					"temp",
																					"humid1",
																					"humid2",
																					"press1",
																					"press2",
																					"rain_10min",
																					"rain_hour",
																					"rain_day",
																					"water_level",
																					"move_rain_10min",
																					"move_rain_hour",
																					"move_rain_day",
																					"rain_1min",
																					"rain_rui",
																					"sun_day",
																				)

													);

		var $monthly_mode = false;
		var $yearly_mode 	= false;

		//ＦＴＰオブジェクト生成
		function CreateFTP($pnt_id){
			if(!$this->load_flg)	$this->LoadLoggerInfo($pnt_id);

			$ip		= $this->setting["lgr_ftp_ip"];
			$id 	=	$this->setting["lgr_ftp_id"];
			$pwd	=	$this->setting["lgr_ftp_pwd"];
			$dir	=	$this->setting["lgr_ftp_dir"];

			//FTP接続
			$this->ftp = new ftp();
			$this->ftp->timeout		= 10;
			$this->ftp->stimeout	= 2;

			if(is_object($this->log))	$this->log->logPrint("FTP Connect Trying",REC_DEBUG,__LINE__,__FILE__);
			if($this->ftp->ftp_connect($ip)){
				if(is_object($this->log))	$this->log->logPrint("FTP Connect",REC_DEBUG,__LINE__,__FILE__);
				if($this->ftp->ftp_login($id,$pwd)){
					if($dir!=""){
						if($this->ftp->ftp_chdir($dir)){
							if(is_object($this->log))	$this->log->logPrint("FTP Login Current => ".$this->ftp->ftp_pwd(),REC_DEBUG,__LINE__,__FILE__);
						}else{
							if(is_object($this->log))	$this->log->logPrint("FTP Change Directory Error",REC_SOCK,__LINE__,__FILE__);
						}
					}else{
						if(is_object($this->log))	$this->log->logPrint("FTP Login Current => ".$this->ftp->ftp_pwd(),REC_DEBUG,__LINE__,__FILE__);
					}
				}else{
					if(is_object($this->log))	$this->log->logPrint("FTP Login Error",REC_SOCK,__LINE__,__FILE__);
					return false;
				}
			}else{
				if(is_object($this->log))	$this->log->logPrint("FTP Connect Error",REC_SOCK,__LINE__,__FILE__);
				return false;
			}
			return true;
		}


		// .locファイル用ＦＴＰオブジェクト生成
		// (通常FTPデータファイルとディレクトリが異なる為、別途作成。)
		function CreateFTP_loc(){
			if(!$this->load_flg)	$this->LoadLoggerInfo($pnt_id);

			$ip		= $this->setting["lgr_ftp_ip"];
			$id 	=	$this->setting["lgr_ftp_id"];
			$pwd	=	$this->setting["lgr_ftp_pwd"];
			$dir	=	$this->setting["lgr_ftp_dir"];

			//FTP接続
			$this->ftp_loc = new ftp();
			$this->ftp_loc->timeout		= 10;
			$this->ftp_loc->stimeout	= 2;

			if(is_object($this->log))	$this->log->logPrint("FTP Connect Trying",REC_DEBUG,__LINE__,__FILE__);
			if($this->ftp_loc->ftp_connect($ip)){
				if(is_object($this->log))	$this->log->logPrint("FTP Connect",REC_DEBUG,__LINE__,__FILE__);
				if($this->ftp_loc->ftp_login($id,$pwd)){
					if(is_object($this->log))	$this->log->logPrint("FTP Login Current => ".$this->ftp_loc->ftp_pwd(),REC_DEBUG,__LINE__,__FILE__);
				}else{
					if(is_object($this->log))	$this->log->logPrint("FTP Login Error",REC_SOCK,__LINE__,__FILE__);
					return false;
				}
			}else{
				if(is_object($this->log))	$this->log->logPrint("FTP Connect Error",REC_SOCK,__LINE__,__FILE__);
				return false;
			}
			return true;
		}

		/**
		  * FTPサーバのlocファイルの状態を確認し、書き換え完了していない場合（中身が1でない）は読み込みを行わない。
		  *
		  * @param String $filename locファイル名
		  * @return boolean ファイルの状態(true: 書き換え完了、false: 書き換え中もしくはファイルアクセスエラー)
		  **/
		 var $AryLoc = array();
		function checkWriteStatus($filename) {
			//同一処理内でロックファイルを2重に読まない様に対応
			if($this->AryLoc[$filename]){
				if(is_object($this->log))	$this->log->logPrint("recv_loc Checked[{$filename}]",REC_SOCK,__LINE__,__FILE__);
				return true;
			}

			if(!$this->CreateFTP_loc()) return false;
//			$localfile	= XML_DIR.$pnt_id."_".$filename;
			$localfile	= XML_DIR.$filename;
			$remotefile	= $filename;
//			@unlink($localfile);
			$ret = $this->ftp_loc->ftp_get($localfile, $remotefile);
			if(!$ret){
				if(is_object($this->log))	$this->log->logPrint("FTP_loc GET ERROR[{$filename}]",REC_SOCK,__LINE__,__FILE__);
				return false;
			}
			usleep(100000);
			if(!$line=@file($localfile)){
				if(is_object($this->log))	$this->log->logPrint("Read_loc Error[{$filename}]",REC_SOCK,__LINE__,__FILE__);
				return false;
			}
			$recv = trim($line[0]);
			if(is_object($this->log))	$this->log->logPrint("recv_loc:[".$recv."][{$filename}]",REC_SOCK,__LINE__,__FILE__);
			if(trim($line[0])==""){
				if(is_object($this->log))	$this->log->logPrint("recv_loc:No Data [{$filename}]",REC_SOCK,__LINE__,__FILE__);
				return false;
			}
			if(intval($recv) < 1){
				if(is_object($this->log))	$this->log->logPrint("recv:Now Writing... [{$filename}]",REC_SOCK,__LINE__,__FILE__);
				return false;
			}
			@unlink($localfile);
			$this->AryLoc[$filename] = true;
			return true;
		}

		//通信チェック
		function CheckConnection($pnt_id){
			if(!$this->load_flg) $this->LoadLoggerInfo($pnt_id);
			if(!$this->CreateFTP($pnt_id)) return false;

			$filename		= "GENZAI.dat";
			$localfile	= XML_DIR.$pnt_id."_".$filename;
			$remotefile	= $filename;
			@unlink($localfile);
			$ret = $this->ftp->ftp_get($localfile,$remotefile);
			$this->ftp->ftp_quit();
			if(!$ret){
				if(is_object($this->log))	$this->log->logPrint("FTP GET ERROR[{$filename}]",REC_SOCK,__LINE__,__FILE__);
				return false;
			}
			if(!$line=@file($localfile)){
				if(is_object($this->log))	$this->log->logPrint("Read Error[{$filename}]",REC_SOCK,__LINE__,__FILE__);
				return false;
			}
			if(is_object($this->log))	$this->log->logPrint("recv:[".$line[0]."][{$filename}]",REC_SOCK,__LINE__,__FILE__);
			$buf		= trim($line[0]);
			$buf		= $this->ClearBuf($buf);
			@unlink($localfile);
			return $buf;
		}

		function GetRuiRain($pnt_id,$curTime){
			if(!$this->load_flg)	$this->LoadLoggerInfo($pnt_id);
			if(!$this->CreateFTP($pnt_id)) return false;

			// FTPサーバのlocファイルの状態を確認し、書き換え完了していない場合（中身が1でない）は読み込みを行わない。
			if (!$this->checkWriteStatus("GENZAI.loc")) return false;

			$rain_update_flg = false;

			//累計雨量データの取得
			$filename		= "GENZAI_RK.dat";
			$localfile	= XML_DIR.$pnt_id."_".$filename;
			$remotefile	= $filename;
			@unlink($localfile);
			$ret = $this->ftp->ftp_get($localfile,$remotefile);
			$this->ftp->ftp_quit();
			if($ret){
				if($line=@file($localfile)){
					if(is_object($this->log))	$this->log->logPrint("recv:[".$line[0]."][{$filename}]",REC_SOCK,__LINE__,__FILE__);
					if(trim($line[0])!=""){
						$buf		= trim($line[0]);
						$buf		= $this->ClearBuf($buf);
						$AryTmp =	$this->ClearBuf2($buf);

						$t = $AryTmp[0];
					 	$t = str_replace("/","",$t);
					 	$t = str_replace(" ","",$t);
					 	$t = str_replace(":","",$t);

					 	//特殊対応
					 	if($t=="2400"){
					 		$t = date("Ymd",$curTime-86400).$t;
					 	}else{
					 		$t = date("Ymd",$curTime).$t;
					 	}

					 	$t = DateConvert3($t);
					 	$curRain		= $t;
					 	$rain_rui		= trim($AryTmp[1]);

					 	if(is_numeric($rain_rui)){
					 		$date_value = date("Y/m/d H:i:s",$curRain);
							//累計雨量テーブルより降雨中データ取得
							$dbs 		= new CONN;
							$strSql = "SELECT * FROM t500_dat WHERE t500_pnt_id = '{$pnt_id}' AND t500_edt IS NULL;";
							$row 		= $dbs->Query($strSql);
							if(is_object($this->log))	$this->log->logPrint("Rain t500_dat => ".print_r($row,TRUE),REC_DEBUG,__LINE__,__FILE__);

						 	if($rain_rui>0){
						 		if($row["t500_pnt_id"]==""){
						 			//開始時刻を再取得
						 			if($this->CreateFTP($pnt_id)){
										$filename		= date("Ym",$curRain)."_RK.dat";
										$localfile	= XML_DIR.$pnt_id."_".$filename;
										$remotefile	= $filename;
										@unlink($localfile);
										$ret = $this->ftp->ftp_get($localfile,$remotefile);
										$this->ftp->ftp_quit();
										if($line=@file($localfile)){
											if(is_object($this->log))	$this->log->logPrint("recv:[".join("",$line)."][{$filename}]",REC_SOCK,__LINE__,__FILE__);
											foreach($line as $val){
												$buf		= $this->ClearBuf(trim($val));
												$AryTmp =	$this->ClearBuf2($buf);
												if($AryTmp[1]==""){
													$date_value = $AryTmp[0];
												}
											}
										}
									}
						 			//累計雨量計測開始
						 			$strSql = "INSERT INTO t500_dat VALUES(DEFAULT,'{$pnt_id}','{$date_value}',NULL,'{$date_value}',{$rain_rui});";
						 		}else{
						 			$old = $row["t500_value"] * 1;
						 			$new = $rain_rui * 1;
						 			if($old!=$new){
							 			//累計雨量計測継続
							 			$strSql = "UPDATE t500_dat SET t500_value = {$rain_rui},t500_last = '{$date_value}' WHERE t500_pnt_id = '{$pnt_id}' AND t500_edt IS NULL;";
							 		}else{
							 			$strSql = "";
							 		}
						 		}
						 	}else{
						 		if($row["t500_pnt_id"]!=""){
						 			//累計雨量計測停止
						 			$strSql = "UPDATE t500_dat SET t500_edt = '{$date_value}' WHERE t500_pnt_id = '{$pnt_id}' AND t500_edt IS NULL;";
						 			$rain_update_flg = true;
						 		}
						 	}
						 	if($strSql!=""){
						 		if(is_object($this->log))	$this->log->logPrint($strSql,REC_DEBUG,__LINE__,__FILE__);
								if(!$dbs->Execute($strSql)){
									if(is_object($this->log))	$this->log->logPrint($dbs->getError(),REC_NOTE,__LINE__,__FILE__);
									$rain_rui = "";
								}
								if($rain_update_flg){
									$this->GetRuiRainUpdate($pnt_id,$curTime);
								}
							}
						}
						$rain_rui = is_numeric($rain_rui) ? number_format($rain_rui,1,".", "") : "";
					}else{
						if(is_object($this->log))	$this->log->logPrint("recv:No Data [{$filename}]",REC_SOCK,__LINE__,__FILE__);
					}
				}else{
					if(is_object($this->log))	$this->log->logPrint("Read Error[{$filename}]",REC_SOCK,__LINE__,__FILE__);
				}
			}else{
				if(is_object($this->log))	$this->log->logPrint("FTP GET ERROR[{$filename}]",REC_SOCK,__LINE__,__FILE__);
			}
			return $rain_rui;
		}

		function GetRuiRainUpdate($pnt_id,$curTime){
			if(!$this->load_flg)	$this->LoadLoggerInfo($pnt_id);
			if(!$this->CreateFTP($pnt_id)) return false;

			$date		= DateConvert($curTime);
			$year		= $date["year"];
			$month	= $date["month"];

			$AryRain = array();
			$Ary10MRain = array();

			//累計雨量データの取得
			$filename		= $year.$month."_RK.dat";
			$localfile	= XML_DIR.$pnt_id."_".$filename;
			$remotefile	= $filename;
			@unlink($localfile);
			$ret = $this->ftp->ftp_get($localfile,$remotefile);
			if($ret){
				if($line=@file($localfile)){
					if(is_object($this->log))	$this->log->logPrint("recv:[".join("",$line)."][{$filename}]",REC_SOCK,__LINE__,__FILE__);
					foreach($line as $val){
						$buf		= $this->ClearBuf(trim($val));
						$AryTmp =	$this->ClearBuf2($buf);
						$AryRain[]["sdt"] = $AryTmp[0];
						$AryRain[count($AryRain)-1]["edt"] = $AryTmp[1];
						$AryDate[ substr($AryTmp[1],0,10) ] = "";
					}
					foreach($AryDate as $trg => $dmy){
						$filename		= str_replace("/","",$trg)."_10MRK.dat";
						$localfile	= XML_DIR.$pnt_id."_".$filename;
						$remotefile	= $filename;
						@unlink($localfile);

						$ret = $this->ftp->ftp_get($localfile,$remotefile);
						if($line=@file($localfile)){
							if(is_object($this->log))	$this->log->logPrint("///// GetRuiRainUpdate ///// Get OK[".join($line)."][".$remotefile."]",REC_DEBUG,__LINE__,__FILE__);
							foreach($line as $val){
								$buf		= $this->ClearBuf(trim($val));
								$AryTmp =	$this->ClearBuf2($buf);
								$Ary10MRain[$trg." ".$AryTmp[0]] = $AryTmp[1];
							}
						}else{
							if(is_object($this->log))	$this->log->logPrint("///// GetRuiRainUpdate ///// Get NG[".$remotefile."]",REC_DEBUG,__LINE__,__FILE__);
						}
						@unlink($localfile);
					}
					for($i=0;$i<count($AryRain);$i++){
						$trg = $AryRain[$i]["edt"];
						if(substr($trg,-1)!="0"){
							$t = strtotime($trg);
							$t += (10-substr($trg,-1))*60;
						}else{
							$t = strtotime($trg);
						}
						$date = DateConvert($t);
						$trg = $date["year"]."/".$date["month"]."/".$date["day"]." ".$date["hour"].":".$date["min"];
						$AryRain[$i]["value"] = $Ary10MRain[$trg];
					}

					$dbs = new CONN;
					$strSql = "DELETE FROM t500_dat WHERE t500_pnt_id = '{$pnt_id}' AND (( EXTRACT(YEAR FROM t500_sdt)={$year} AND EXTRACT(MONTH FROM t500_sdt)={$month} ) OR (EXTRACT(YEAR FROM t500_last)={$year} AND EXTRACT(MONTH FROM t500_last)={$month}));";
					if(is_object($this->log))	$this->log->logPrint("///// GetRuiRainUpdate /////".$strSql,REC_DEBUG,__LINE__,__FILE__);
					$dbs->Execute($strSql);
					foreach($AryRain as $val){
						$sdt = str_replace("/","",$val["sdt"]);
						$sdt = str_replace(":","",$sdt);
						$sdt = str_replace(" ","",$sdt);
						$t = DateConvert3($sdt);
						$sdt = date("Y/m/d H:i",$t);

						$edt = str_replace("/","",$val["edt"]);
						$edt = str_replace(":","",$edt);
						$edt = str_replace(" ","",$edt);
						$t = DateConvert3($edt);
						$edt = date("Y/m/d H:i",$t);
						$strSql = "INSERT INTO t500_dat VALUES(DEFAULT,'{$pnt_id}','{$sdt}','{$edt}','{$edt}',".$val["value"].");";
						if(is_object($this->log))	$this->log->logPrint("///// GetRuiRainUpdate /////".$strSql,REC_DEBUG,__LINE__,__FILE__);
						$dbs->Execute($strSql);
					}
				}else{
					if(is_object($this->log))	$this->log->logPrint("Read Error[{$filename}]",REC_SOCK,__LINE__,__FILE__);
				}
				@unlink($localfile);
			}else{
				if(is_object($this->log))	$this->log->logPrint("FTP GET ERROR[{$filename}]",REC_SOCK,__LINE__,__FILE__);
			}
			$this->ftp->ftp_quit();
			return $rain_rui;
		}

		//現在データ取得処理
		function GetPresentData($pnt_id,&$curTime,&$args){
			if(!$this->load_flg)	$this->LoadLoggerInfo($pnt_id);
			if(!$this->CreateFTP($pnt_id)) return false;

			// FTPサーバのlocファイルの状態を確認し、書き換え完了していない場合（中身が1でない）は読み込みを行わない。
			if (!$this->checkWriteStatus("GENZAI.loc")) return false;

			$filename		= "GENZAI.dat";
			$localfile	= XML_DIR.$pnt_id."_".$filename;
			$remotefile	= $filename;
			@unlink($localfile);
			$ret = $this->ftp->ftp_get($localfile,$remotefile);
			if(!$ret){
				if(is_object($this->log))	$this->log->logPrint("FTP GET ERROR[{$filename}]",REC_SOCK,__LINE__,__FILE__);
				return false;
			}
			if(!$line=@file($localfile)){
				if(is_object($this->log))	$this->log->logPrint("Read Error[{$filename}]",REC_SOCK,__LINE__,__FILE__);
				return false;
			}
			if(is_object($this->log))	$this->log->logPrint("recv:[".$line[0]."][{$filename}]",REC_SOCK,__LINE__,__FILE__);
			if(trim($line[0])==""){
				if(is_object($this->log))	$this->log->logPrint("recv:No Data [{$filename}]",REC_SOCK,__LINE__,__FILE__);
				return false;
			}
			@unlink($localfile);

			$buf		= trim($line[0]);
			$buf		= $this->ClearBuf($buf);
			$AryTmp =	$this->ClearBuf2($buf);

			$t = $AryTmp[0];
		 	$t = str_replace("/","",$t);
		 	$t = str_replace(" ","",$t);
		 	$t = str_replace(":","",$t);
		 	$t = DateConvert3($t);
		 	if(date("Y/m/d H:i",$t)!=date("Y/m/d H:i",$curTime)){
				$curTime = $t;
		 	}

		 	//$AryWind = $this->GetMaxWind($pnt_id,$curTime,$AryTmp[3],$AryTmp[4]);
			if(array_key_exists("avg_ws",$args))					$args["avg_ws"]						=	is_numeric($AryTmp[2])					? number_format($AryTmp[2],1,".","")	: "";
			if(array_key_exists("avg_wd",$args))					$args["avg_wd"]						=	is_numeric($AryTmp[1])					? sprintf("%02d",$AryTmp[1]) 					: "";
			//if(array_key_exists("max_ws",$args))					$args["max_ws"]						=	is_numeric($AryWind["max_ws"])	? $AryWind["max_ws"]									: "";
			//if(array_key_exists("max_wd",$args))					$args["max_wd"]						=	is_numeric($AryWind["max_wd"])	? $AryWind["max_wd"]									: "";
			if(array_key_exists("max_ws",$args))					$args["max_ws"]						=	is_numeric($AryTmp[3])					? number_format($AryTmp[3],1,".","")	: "";
			if(array_key_exists("max_wd",$args))					$args["max_wd"]						=	is_numeric($AryTmp[4])					? sprintf("%02d",$AryTmp[4]) 					: "";
			if(array_key_exists("temp",$args))						$args["temp"]							=	is_numeric($AryTmp[6]) 					? number_format($AryTmp[6],1,	".","")	: "";
			if(array_key_exists("humid1",$args))					$args["humid1"]						=	is_numeric($AryTmp[12])					? number_format($AryTmp[12],1,".","") : "";
			if(array_key_exists("humid2",$args))					$args["humid2"]						=	is_numeric($AryTmp[52])					? number_format($AryTmp[52],1,".","") : "";
			if(array_key_exists("press1",$args))					$args["press1"]						=	is_numeric($AryTmp[14])					? number_format($AryTmp[14],1,".","") : "";
			if(array_key_exists("move_rain_10min",$args))	$args["move_rain_10min"]	=	is_numeric($AryTmp[7]) 					? number_format($AryTmp[7],1,".","")  : "";
			if(array_key_exists("move_rain_hour",$args))	$args["move_rain_hour"]		=	is_numeric($AryTmp[8]) 					? number_format($AryTmp[8],1,".","")  : "";
			if(array_key_exists("sun_day",$args))					$args["sun_day"]					=	is_numeric($AryTmp[9]) 					? number_format($AryTmp[9],0,".","")  : "";
			if(array_key_exists("snow_level",$args))			$args["snow_level"]				=	is_numeric($AryTmp[11]) 				? number_format($AryTmp[11],0,".","")	: "";
			if(array_key_exists("snow_hour",$args))				$args["snow_hour"]				=	is_numeric($AryTmp[58]) 				? number_format($AryTmp[58],0,".","")	: "";
			if(array_key_exists("rain_day",$args))				$args["rain_day"]					=	is_numeric($AryTmp[21]) 				? number_format($AryTmp[21],1,".","")  : "";
			if(array_key_exists("water_level",$args))			$args["water_level"]			=	is_numeric($AryTmp[22]) 				? number_format($AryTmp[22],2,".","") : "";
			if(array_key_exists("tide_level",$args))			$args["tide_level"]				=	is_numeric($AryTmp[23]) 				? number_format($AryTmp[23],2,".","") : "";
			if(array_key_exists("radiation",$args))				$args["radiation"]				=	is_numeric($AryTmp[53]) 				? number_format($AryTmp[53],2,".","") : "";
			if(array_key_exists("rain_rui",$args)) 				$args["rain_rui"] 				= $this->GetRuiRain($pnt_id,$curTime);
			if(array_key_exists("press2",$args)) 				$args["press2"] 				= is_numeric($AryTmp[65]) 					? number_format($AryTmp[65],2,".","")  : "";
			if(array_key_exists("sun_day",$args)) 				$args["sun_day"] 				= is_numeric($AryTmp[9]) 					? number_format($AryTmp[9],0,".","")  : "";
			if(array_key_exists("sun_month",$args)) 				$args["sun_month"] 				= is_numeric($AryTmp[10]) 					? number_format($AryTmp[10],0,".","")  : "";

////			//累計雨量データの取得
////			$filename		= "GENZAI_RK.dat";
////			$localfile	= XML_DIR.$pnt_id."_".$filename;
////			$remotefile	= $filename;
////			@unlink($localfile);
////			$ret = $this->ftp->ftp_get($localfile,$remotefile);
////			$this->ftp->ftp_quit();
////			if($ret){
////				if($line=@file($localfile)){
////					if(is_object($this->log))	$this->log->logPrint("recv:[".$line[0]."][{$filename}]",REC_SOCK,__LINE__,__FILE__);
////					if(trim($line[0])!=""){
////						$buf		= trim($line[0]);
////						$buf		= $this->ClearBuf($buf);
////						$AryTmp =	$this->ClearBuf2($buf);
////
////						$t = $AryTmp[0];
////					 	$t = str_replace("/","",$t);
////					 	$t = str_replace(" ","",$t);
////					 	$t = str_replace(":","",$t);
////					 	$t = DateConvert3($t);
////					 	$curRain		= $t;
////					 	$rain_rui		= trim($AryTmp[1]);
////
////					 	if(is_numeric($rain_rui)){
////					 		$date_value = date("Y/m/d H:i:s",$curRain);
////							//累計雨量テーブルより降雨中データ取得
////							$dbs 		= new CONN;
////							$strSql = "SELECT * FROM t500_dat WHERE t500_pnt_id = '{$pnt_id}' AND t500_edt IS NULL;";
////							$row 		= $dbs->Query($strSql);
////							if(is_object($this->log))	$this->log->logPrint("Rain t500_dat => ".print_r($row,TRUE),REC_DEBUG,__LINE__,__FILE__);
////
////						 	if($rain_rui>0){
////						 		if($row["t500_pnt_id"]==""){
////						 			//累計雨量計測開始
////						 			$strSql = "INSERT INTO t500_dat VALUES(DEFAULT,'{$pnt_id}','{$date_value}',NULL,'{$date_value}',{$rain_rui});";
////						 		}else{
////						 			$old = $row["t500_value"] * 1;
////						 			$new = $rain_rui * 1;
////						 			if($old!=$new){
////							 			//累計雨量計測継続
////							 			$strSql = "UPDATE t500_dat SET t500_value = {$rain_rui},t500_last = '{$date_value}' WHERE t500_pnt_id = '{$pnt_id}' AND t500_edt IS NULL;";
////							 		}else{
////							 			$strSql = "";
////							 		}
////						 		}
////						 	}else{
////						 		if($row["t500_pnt_id"]!=""){
////						 			//累計雨量計測停止
////						 			$strSql = "UPDATE t500_dat SET t500_edt = '{$date_value}' WHERE t500_pnt_id = '{$pnt_id}' AND t500_edt IS NULL;";
////						 		}
////						 	}
////						 	if($strSql!=""){
////						 		if(is_object($this->log))	$this->log->logPrint($strSql,REC_DEBUG,__LINE__,__FILE__);
////								if(!$dbs->Execute($strSql)){
////									if(is_object($this->log))	$this->log->logPrint($dbs->getError(),REC_NOTE,__LINE__,__FILE__);
////									$rain_rui = "";
////								}
////							}
////						}
////						if(array_key_exists("rain_rui",$args)) $args["rain_rui"] = is_numeric($rain_rui) ? number_format($rain_rui,1,".", "") : "";
////					}else{
////						if(is_object($this->log))	$this->log->logPrint("recv:No Data [{$filename}]",REC_SOCK,__LINE__,__FILE__);
////					}
////				}else{
////					if(is_object($this->log))	$this->log->logPrint("Read Error[{$filename}]",REC_SOCK,__LINE__,__FILE__);
////				}
////			}else{
////				if(is_object($this->log))	$this->log->logPrint("FTP GET ERROR[{$filename}]",REC_SOCK,__LINE__,__FILE__);
////			}
			if(is_object($this->log))	$this->log->logPrint($this->id." GetPresentData ".print_r($args,TRUE),REC_DEBUG,__LINE__,__FILE__);
			@unlink($localfile);

			$this->ftp->ftp_quit();

			return true;
		}

		//現在データから10分移動最大瞬間風速風向の算出
		function GetMaxWind($pnt_id,$curTime,$ws,$wd){
			$path		= ROOT_INC."wind_{$pnt_id}.dat";
			$update = false;

			if($line=@file($path)){
				$tmp = explode(",",$line[0]);
				$time 	= $tmp[0];
				$max_ws = $tmp[1];
				$max_wd = $tmp[2]
				;
				$date		= DateConvert($time);
				$max_dt = $date["date"];

				print "++++GetMaxWind++++ ".date("Y/m/d H:i",($curTime-600))."<=".date("Y/m/d H:i",$time)."\n";
				if(($curTime-600)<=$time){
					print "++++GetMaxWind++++ ".date("Y/m/d H:i",$time)."\n";
					if($max_ws<$ws){
						$date		= DateConvert($curTime);
						$max_dt = $date["date"];
						$max_ws = $ws;
						$max_wd = $wd;
						$update = true;
					}
				}else{
					print "++++GetMaxWind++++ ".date("Y/m/d H:i",$time)." orver\n";
					$date		= DateConvert($curTime);
					$max_dt = $date["date"];
					$max_ws = $ws;
					$max_wd = $wd;
					$update = true;
				}
			}else{
				$date		= DateConvert($curTime);
				$max_dt = $date["date"];
				$max_ws = $ws;
				$max_wd = $wd;
				$update = true;
			}

			if($update){
				print "++++GetMaxWind++++ "."max_wind update\n";
				if($fp=@fopen($path,"w")){
					$buf = "{$curTime},{$ws},{$wd}";
					print "++++GetMaxWind++++ {$buf}\n";
					fwrite($fp,$buf);
					fclose($fp);
				}else{
					print $path." Open Error\n";
				}
			}
			$AryTmp = array();
			$AryTmp["date"]		= $max_dt;
			$AryTmp["max_ws"] = $max_ws;
			$AryTmp["max_wd"] = $max_wd;
			if(is_object($this->log))	$this->log->logPrint("++++GetMaxWind++++ ".print_r($AryTmp,TRUE),REC_DEBUG,__LINE__,__FILE__);
			return $AryTmp;
		}

		//時報データ取得処理 [2009/02/10]
		function GetMinutelyData($pnt_id,$curTime,&$args){
			$date	= DateConvert($curTime-3600);
			$trg1	= $date["year"].$date["month"].$date["day"]."0010";
			$date	= DateConvert($curTime);
			$trg2	= $date["year"].$date["month"].$date["day"]."2400";
			$ret = $this->GetLoggerMinutelyCollection($pnt_id,$trg1,$trg2);
			if(!$ret) return false;

			$data = new Data($this->log);
			foreach($this->AryColData as $key => $val){
				$time = DateConvert3($key);
				$data->SaveData($pnt_id,"100",$time,$val,"",false);
			}
			return true;
		}
		//時報データ手動収集（連想配列に格納したデータを取得) [2009/02/10]
		function GetMinutelyCollectionData($pnt_id,$curTime,&$args){
			$date = DateConvert($curTime);
			if(is_array($this->AryColData[ $date["date"] ])){
				$args = $this->AryColData[ $date["date"] ];
				return true;
			}else{
				if(is_object($this->log))	$this->log->logPrint($this->id." GetMinutelyCollectionData No Data",REC_DEBUG,__LINE__,__FILE__);
				return false;
			}
		}
		//時報データ手動収集（取得データを連想配列に格納する [2009/02/10]
		function GetLoggerMinutelyCollection($pnt_id,$trg1,$trg2=""){
			if(!$this->load_flg)	$this->LoadLoggerInfo($pnt_id);
			if(!$this->CreateFTP($pnt_id)) return false;

			//ロックファイル
			if(!$this->checkWriteStatus("10M.loc")) return false;

			//データ取得フィールド定義
			$data = new Data($log);
			$rec	= $data->GetDataField("100");

			if(is_object($this->log))	$this->log->logPrint("GetLoggerMinutelyCollection [{$trg1}] => [{$trg2}]",REC_SOCK,__LINE__,__FILE__);

			//格納用連想配列初期化
			$this->AryColData = array();

			if($trg2!=""){
				$time1 = DateConvert3($trg1);
				$time2 = DateConvert3($trg2);
			}else{
				$time1 = DateConvert3($trg1);
				$time2 = $time1;
			}

			$t1 = $time1;
			$t2 = $time2;
			while($t1<=$t2){

				if($wk!=date("Ymd",$t1)){
				//if($wk!=date("Ymd",$t1) && $this->checkWriteStatus("10M.loc")){
					$filename		= date("Ymd",$t1)."_10M.dat";
					$localfile	= XML_DIR.$pnt_id."_".$filename;
					$remotefile	= $filename;
					@unlink($localfile);
					$ret = $this->ftp->ftp_get($localfile,$remotefile);
					if($ret){
						if($line=@file($localfile)){
							if(is_object($this->log))	$this->log->logPrint("recv:[".join("",$line)."][{$filename}]",REC_SOCK,__LINE__,__FILE__);

							foreach($line as $value){
								$value	= str_replace("\r\n","",$value);
								$tmp		= explode(",",$value);
								$buf		= $this->ClearBuf($value);
								$AryTmp	=	$this->ClearBuf2($buf);

								$args = $rec;

								if(array_key_exists("avg_ws",$args))					$args["avg_ws"]						=	is_numeric($AryTmp[2])		? number_format($AryTmp[2],	1,".", "")		: "";
								if(array_key_exists("avg_wd",$args))					$args["avg_wd"]						=	is_numeric($AryTmp[1])		? sprintf("%02d",$AryTmp[1]) 							: "";
								if(array_key_exists("max_ws",$args))					$args["max_ws"]						=	is_numeric($AryTmp[3])		? number_format($AryTmp[3],	1,".", "")  	: "";
								if(array_key_exists("max_wd",$args))					$args["max_wd"] 					=	is_numeric($AryTmp[4])		? sprintf("%02d",$AryTmp[4])							: "";
								if(array_key_exists("temp",$args))						$args["temp"]							=	is_numeric($AryTmp[6]) 		? number_format($AryTmp[6],	1,	".", "") 	: "";
								if(array_key_exists("humid1",$args))					$args["humid1"]						=	is_numeric($AryTmp[12]) 	? number_format($AryTmp[12], 1,".", "") 	: "";
								if(array_key_exists("humid2",$args))					$args["humid2"]						=	is_numeric($AryTmp[52]) 	? number_format($AryTmp[52], 1,".", "") 	: "";
								if(array_key_exists("press1",$args))					$args["press1"]						=	is_numeric($AryTmp[14]) 	? number_format($AryTmp[14],1,".", "")  	: "";
								if(array_key_exists("rain_10min",$args))			$args["rain_10min"]				=	is_numeric($AryTmp[7]) 		? number_format($AryTmp[7],1,".", "")  		: "";
								if(array_key_exists("rain_hour",$args))				$args["rain_hour"]				=	is_numeric($AryTmp[8]) 		? number_format($AryTmp[8],1,".", "")  		: "";
								if(array_key_exists("rain_day",$args))				$args["rain_day"]					=	is_numeric($AryTmp[21]) 	? number_format($AryTmp[21],1,".", "")  	: "";
								if(array_key_exists("sun_day",$args))					$args["sun_day"]					=	is_numeric($AryTmp[9]) 		? number_format($AryTmp[9],0,".", "")  		: "";
								if(array_key_exists("snow_level",$args))			$args["snow_level"]				=	is_numeric($AryTmp[11]) 	? number_format($AryTmp[11],0,".","")	: "";
								if(array_key_exists("snow_hour",$args))				$args["snow_hour"]				=	is_numeric($AryTmp[58]) 	? number_format($AryTmp[58],0,".","")	: "";
								if(array_key_exists("tide_level",$args))			$args["tide_level"]				=	is_numeric($AryTmp[23]) 	? number_format($AryTmp[23],2, ".", "")  	: "";
								if(array_key_exists("max_tide_level",$args))	$args["max_tide_level"]		=	is_numeric($AryTmp[43]) 	? number_format($AryTmp[43],2, ".", "")  	: "";
								if(array_key_exists("min_tide_level",$args))	$args["min_tide_level"]		=	is_numeric($AryTmp[45]) 	? number_format($AryTmp[45],2, ".", "")  	: "";
								if(array_key_exists("water_level",$args))			$args["water_level"]			=	is_numeric($AryTmp[22]) 	? number_format($AryTmp[22],2, ".", "")  	: "";
								if(array_key_exists("max_water_level",$args))	$args["max_water_level"]	=	is_numeric($AryTmp[39]) 	? number_format($AryTmp[39],2, ".", "")  	: "";
								if(array_key_exists("min_water_level",$args))	$args["min_water_level"]	=	is_numeric($AryTmp[41]) 	? number_format($AryTmp[41],2, ".", "")  	: "";
								if(array_key_exists("radiation",$args))				$args["radiation"]				=	is_numeric($AryTmp[53]) 	? number_format($AryTmp[53],2,".","") 		: "";

								if(array_key_exists("press2",$args))				$args["press2"]				=	is_numeric($AryTmp[65]) 	? number_format($AryTmp[65],2,".","") 		: "";
								if(array_key_exists("sun_day",$args))				$args["sun_day"]				=	is_numeric($AryTmp[9]) 	? number_format($AryTmp[9],2,".","") 		: "";
								if(array_key_exists("sun_month",$args))				$args["sun_month"]				=	is_numeric($AryTmp[10]) 	? number_format($AryTmp[10],2,".","") 		: "";


								$this->AryColData[ date("Ymd",$t1).str_replace(":","",$AryTmp[0]) ] = $args;
							}
						}else{
							if(is_object($this->log))	$this->log->logPrint("Read Error[{$filename}]",REC_SOCK,__LINE__,__FILE__);
						}
						@unlink($localfile);
					}else{
						if(is_object($this->log))	$this->log->logPrint("FTP GET ERROR[{$filename}]",REC_SOCK,__LINE__,__FILE__);
					}
					$wk = date("Ymd",$t1);
				}
				$t1 += 600;
				if($t1>time()) break;
			}
			$this->ftp->ftp_quit();
			return true;
		}

		//日報データ取得処理 [2009/02/10]
		function GetDailyData($pnt_id,$curTime,&$args){
			$date	= DateConvert($curTime-7200);
			$trg1	= $date["year"].$date["month"].$date["day"]."0010";
			$date	= DateConvert($curTime);
			$trg2	= $date["year"].$date["month"].$date["day"]."2400";
			$this->GetLoggerDailyCollection($pnt_id,$trg1,$trg2);

			$data = new Data($this->log);
			foreach($this->AryColData as $key => $val){
				$time = DateConvert3($key);
				$data->SaveData($pnt_id,"200",$time,$val,"",false);
			}
			return true;
		}
		//日報集計データ取得処理 [2009/02/10]
		function GetDailySumData($pnt_id,$curTime,&$args){
			$date	= DateConvert($curTime-7200);
			$trg1	= $date["year"].$date["month"].$date["day"];
			$date	= DateConvert($curTime);
			$trg2	= $date["year"].$date["month"].$date["day"];
			if(!(is_array($this->AryColSumData[$trg1])&&is_array($this->AryColSumData[$trg2]))){
				$this->GetLoggerDailyCollection($pnt_id,$trg1."0010",$trg2."2400");
			}

			$data = new Data($this->log);
			foreach($this->AryColSumData as $key => $val){
				$d2		= DateConvert2($key);
				$trg2	= $d2["year"].$d2["month"].$d2["day"];
				foreach($val as $sum_no => $rec){
					$data->SaveData($pnt_id,"200",strtotime($d2["year"]."/".$d2["month"]."/".$d2["day"]." 00:01"),$rec,$sum_no,true);
				}
			}
			return true;
		}
		//日報データ手動収集（連想配列に格納したデータを取得） [2009/02/10]
		function GetDailyCollectionData($pnt_id,$curTime,&$args){
			$date	= DateConvert($curTime);
			$trg	= $date["date"];
			foreach($args as $fld => $val){
				$args[$fld] = $this->AryColData[$trg][$fld];
			}
			return true;
		}
		//日報集計データ手動収集（連想配列に格納したデータを取得） [2009/02/10]
		function GetDailyCollectionSumData($pnt_id,$curTime,&$args){
			$date	= DateConvert($curTime);
			$trg	= $date["year"].$date["month"].$date["day"];
			foreach($args as $sum_no => $rec){
				foreach($rec as $fld => $val){
					$args[$sum_no][$fld] = $this->AryColSumData[$trg][$sum_no][$fld];
				}
			}
			return true;
		}
		//日報データ手動収集（取得データを連想配列に格納する） [2009/02/10]
		function GetLoggerDailyCollection($pnt_id,$trg1,$trg2=""){
			if(!$this->load_flg)	$this->LoadLoggerInfo($pnt_id);
			if(!$this->CreateFTP($pnt_id)) return false;

			//データ取得フィールド定義
			$data = new Data($log);
			$rec	= $data->GetDataField("200");


			if(is_object($this->log))	$this->log->logPrint("GetLoggerDailyCollection [{$trg1}] => [{$trg2}]",REC_SOCK,__LINE__,__FILE__);

			//格納用連想配列初期化
			$this->AryColData = array();

			if($trg2!=""){
				$time1 = DateConvert3($trg1);
				$time2 = DateConvert3($trg2);
			}else{
				$time1 = DateConvert3($trg1);
				$time2 = $time1;
			}


			$t1 = $time1;
			$t2 = $time2;
			while($t1<=$t2){

				if($wk!=date("Ymd",$t1)){

					if(!$this->monthly_mode){
						if($this->checkWriteStatus("1H.loc")){
							$filename		= date("Ymd",$t1)."_1H.dat";
							$localfile	= XML_DIR.$pnt_id."_".$filename;
							$remotefile	= $filename;
							@unlink($localfile);
							$ret = $this->ftp->ftp_get($localfile,$remotefile);
							if($ret){
								if($line=@file($localfile)){
									if(is_object($this->log))	$this->log->logPrint("recv:[".join("",$line)."][{$filename}]",REC_SOCK,__LINE__,__FILE__);

									foreach($line as $value){
										$value	= str_replace("\r\n","",$value);
										$tmp		= explode(",",$value);
										$buf		= $this->ClearBuf($value);
										$AryTmp	=	$this->ClearBuf2($buf);

										$args = $rec;

										if(array_key_exists("avg_ws",$args))					$args["avg_ws"]						=	is_numeric($AryTmp[2])		? number_format($AryTmp[2],	1,".", "")		: "";
										if(array_key_exists("avg_wd",$args))					$args["avg_wd"]						=	is_numeric($AryTmp[1])		? sprintf("%02d",$AryTmp[1]) 							: "";
										if(array_key_exists("max_ws",$args))					$args["max_ws"]						=	is_numeric($AryTmp[3])		? number_format($AryTmp[3],	1,".", "")  	: "";
										if(array_key_exists("max_wd",$args))					$args["max_wd"] 					=	is_numeric($AryTmp[4])		? sprintf("%02d",$AryTmp[4])							: "";
										if(array_key_exists("temp",$args))						$args["temp"]							=	is_numeric($AryTmp[9]) 		? number_format($AryTmp[9],	1,	".", "") 	: "";
										if(array_key_exists("humid1",$args))					$args["humid1"]						=	is_numeric($AryTmp[13]) 	? number_format($AryTmp[13], 1,".", "") 	: "";
										if(array_key_exists("humid2",$args))					$args["humid2"]						=	is_numeric($AryTmp[28]) 	? number_format($AryTmp[28], 1,".", "") 	: "";
										if(array_key_exists("press1",$args))					$args["press1"]						=	is_numeric($AryTmp[15]) 	? number_format($AryTmp[15],1,".", "")  	: "";
										if(array_key_exists("press2",$args))					$args["press2"]						=	is_numeric($AryTmp[31]) 	? number_format($AryTmp[31],1,".", "")  	: "";
										if(array_key_exists("max_rain_10min",$args))	$args["max_rain_10min"]		=	is_numeric($AryTmp[17]) 	? number_format($AryTmp[17],1,".", "")  	: "";
										if(array_key_exists("rain_hour",$args))				$args["rain_hour"]				=	is_numeric($AryTmp[10]) 	? number_format($AryTmp[10],1,".", "")  	: "";
										if(array_key_exists("rain_day",$args))				$args["rain_day"]					=	is_numeric($AryTmp[18]) 	? number_format($AryTmp[18],1,".", "")  	: "";
										if(array_key_exists("sun_day",$args))					$args["sun_day"]					=	is_numeric($AryTmp[11]) 	? number_format($AryTmp[11],0,".", "")  	: "";
										if(array_key_exists("snow_level",$args))			$args["snow_level"]				=	is_numeric($AryTmp[12]) 	? number_format($AryTmp[12],0,".", "")  	: "";
										if(array_key_exists("snow_hour",$args))				$args["snow_hour"]				=	is_numeric($AryTmp[30])		? number_format($AryTmp[30],0,".","")			: "";
										if(array_key_exists("tide_level",$args))			$args["tide_level"]				=	is_numeric($AryTmp[22]) 	? number_format($AryTmp[22],2,".", "") 		: "";
										if(array_key_exists("max_tide_level",$args))	$args["max_tide_level"]		=	is_numeric($AryTmp[23]) 	? number_format($AryTmp[23],2,".", "") 		: "";
										if(array_key_exists("min_tide_level",$args))	$args["min_tide_level"]		=	is_numeric($AryTmp[24]) 	? number_format($AryTmp[24],2,".", "") 		: "";
										if(array_key_exists("water_level",$args))			$args["water_level"]			=	is_numeric($AryTmp[19]) 	? number_format($AryTmp[19],2,".", "") 		: "";
										if(array_key_exists("max_water_level",$args))	$args["max_water_level"]	=	is_numeric($AryTmp[20]) 	? number_format($AryTmp[20],2,".", "") 		: "";
										if(array_key_exists("min_water_level",$args))	$args["min_water_level"]	=	is_numeric($AryTmp[21]) 	? number_format($AryTmp[21],2,".", "") 		: "";
										if(array_key_exists("radiation",$args))				$args["radiation"]				=	is_numeric($AryTmp[29]) 	? number_format($AryTmp[29],2,".","") 		: "";
										$this->AryColData[ date("Ymd",$t1).str_replace(":","",$AryTmp[0]) ] = $args;
									}
								}else{
									if(is_object($this->log))	$this->log->logPrint("Read Error[{$filename}]",REC_SOCK,__LINE__,__FILE__);
								}
							}else{
								if(is_object($this->log))	$this->log->logPrint("FTP GET ERROR[{$filename}]",REC_SOCK,__LINE__,__FILE__);
							}
							@unlink($localfile);
						}
					}
					$args = array();

					//集計データ取得
					$filename		= date("Ymd",$t1)."_ST.dat";
					$localfile	= XML_DIR.$pnt_id."_".$filename;
					$remotefile	= $filename;
					@unlink($localfile);
					$ret = $this->ftp->ftp_get($localfile,$remotefile);
					if($ret){
						if($line=@file($localfile)){
							if(is_object($this->log))	$this->log->logPrint("recv:[".join("",$line)."][{$filename}]",REC_SOCK,__LINE__,__FILE__);
							$value	= str_replace("\r\n","",$line[0]);
							$tmp		= explode(",",$value);
							$buf		= $this->ClearBuf($value);
							$AryTmp	=	$this->ClearBuf2($buf);

							//平均風速

							if(array_key_exists("avg_ws",$rec)){
								$args["40"]["avg_ws"]	= is_numeric($AryTmp[1])					? number_format($AryTmp[1],	1,".", "")		: "";
								$args["50"]["avg_ws"]	= is_numeric($AryTmp[5])					? number_format($AryTmp[5],	1,".", "")		: "";
								$args["55"]["avg_ws"]	= strpos(" ".$AryTmp[7],":")>0		? $AryTmp[7]															: "";
							}
							//平均風向
							if(array_key_exists("avg_wd",$rec)){
								$args["40"]["avg_wd"]	= is_numeric($AryTmp[30])					? sprintf("%02d",$AryTmp[30]) 						: "";
								$args["50"]["avg_wd"]	= is_numeric($AryTmp[6])					? sprintf("%02d",$AryTmp[6]) 							: "";
							}
							//最大瞬間風速
							if(array_key_exists("max_ws",$rec)){
								$args["50"]["max_ws"]	= is_numeric($AryTmp[2])					? number_format($AryTmp[2],	1,".", "")		: "";
								$args["55"]["max_ws"]	= strpos(" ".$AryTmp[4],":")>0		? $AryTmp[4]															: "";
							}
							//最大瞬間風向
							if(array_key_exists("max_wd",$rec)){
								$args["50"]["max_wd"]	= is_numeric($AryTmp[3])					? sprintf("%02d",$AryTmp[3]) 							: "";
							}
							//気温
							if(array_key_exists("temp",$rec)){
								$args["40"]["temp"]		= is_numeric($AryTmp[8])					? number_format($AryTmp[8],	1,".", "")		: "";
								$args["50"]["temp"]		= is_numeric($AryTmp[9])					? number_format($AryTmp[9],	1,".", "")		: "";
								$args["55"]["temp"]		= strpos(" ".$AryTmp[10],":")>0		? $AryTmp[10]															: "";
								$args["60"]["temp"]		= is_numeric($AryTmp[11])					? number_format($AryTmp[11],	1,".", "")	: "";
								$args["65"]["temp"]		= strpos(" ".$AryTmp[12],":")>0		? $AryTmp[12]															: "";
							}
							//相対湿度
							if(array_key_exists("humid1",$rec)){
								$args["40"]["humid1"]	= is_numeric($AryTmp[19])				? number_format($AryTmp[19],	1,".", "")	: "";
								$args["50"]["humid1"]	= is_numeric($AryTmp[20])				? number_format($AryTmp[20],	1,".", "")	: "";
								$args["55"]["humid1"]	= strpos(" ".$AryTmp[21],":")>0	? $AryTmp[21]															: "";
								$args["60"]["humid1"]	= is_numeric($AryTmp[22])				? number_format($AryTmp[22],	1,".", "")	: "";
								$args["65"]["humid1"]	= strpos(" ".$AryTmp[23],":")>0	? $AryTmp[23]															: "";
							}
							//実効湿度
							if(array_key_exists("humid2",$rec)){
								$args["40"]["humid2"]	= is_numeric($AryTmp[48])				? number_format($AryTmp[48],	1,".", "")	: "";
							}
							//現地気圧
							if(array_key_exists("press1",$rec)){
								$args["40"]["press1"]	= is_numeric($AryTmp[25])				? number_format($AryTmp[25],	1,".", "")	: "";
								$args["50"]["press1"]	= is_numeric($AryTmp[26])				? number_format($AryTmp[26],	1,".", "")	: "";
								$args["55"]["press1"]	= strpos(" ".$AryTmp[27],":")>0	? $AryTmp[27]															: "";
								$args["60"]["press1"]	= is_numeric($AryTmp[28])				? number_format($AryTmp[28],	1,".", "")	: "";
								$args["65"]["press1"]	= strpos(" ".$AryTmp[29],":")>0	? $AryTmp[29]															: "";
							}
							//現地気圧
							if(array_key_exists("press2",$rec)){
								$args["40"]["press2"]	= is_numeric($AryTmp[51])				? number_format($AryTmp[51],	1,".", "")	: "";
								$args["50"]["press2"]	= is_numeric($AryTmp[52])				? number_format($AryTmp[52],	1,".", "")	: "";
								$args["55"]["press2"]	= strpos(" ".$AryTmp[53],":")>0	? $AryTmp[53]															: "";
								$args["60"]["press2"]	= is_numeric($AryTmp[54])				? number_format($AryTmp[54],	1,".", "")	: "";
								$args["65"]["press2"]	= strpos(" ".$AryTmp[55],":")>0	? $AryTmp[55]															: "";
							}


							//10分最大雨量
							if(array_key_exists("max_rain_10min",$rec)){
								$args["50"]["max_rain_10min"]	= is_numeric($AryTmp[31])													? number_format($AryTmp[31],	1,".", "")	: "";
								$args["55"]["max_rain_10min"]	= (strpos(" ".$AryTmp[32],":")>0&&$AryTmp[31]>0)	? $AryTmp[32]															: "";
							}
							//時間雨量
							if(array_key_exists("rain_hour",$rec)){
								$args["50"]["rain_hour"]			= is_numeric($AryTmp[14])													? number_format($AryTmp[14],	1,".", "")	: "";
								$args["55"]["rain_hour"]			= (strpos(" ".$AryTmp[15],":")>0&&$AryTmp[14]>0)	? $AryTmp[15]															: "";
							}
							//日積算雨量
							if(array_key_exists("rain_day",$rec)){
								$args["70"]["rain_day"]				= is_numeric($AryTmp[13])													? number_format($AryTmp[13],	1,".", "")	: "";
							}
							//日積算日照
							if(array_key_exists("sun_day",$rec)){
								$args["70"]["sun_day"]				= is_numeric($AryTmp[16])													? number_format($AryTmp[16],	0,".", "")	: "";
							}
							//積雪深
							if(array_key_exists("snow_level",$rec)){
								$args["50"]["snow_level"]			= is_numeric($AryTmp[17])													? number_format($AryTmp[17],	0,".", "")	: "";
								$args["55"]["snow_level"]			= (strpos(" ".$AryTmp[18],":")>0&&$AryTmp[17]>0)	? $AryTmp[18]															: "";
							}
							//降雪量
							if(array_key_exists("snow_hour",$rec)){
								$args["50"]["snow_hour"]			= is_numeric($AryTmp[51])													? number_format($AryTmp[51],	0,".", "")	: "";
								$args["55"]["snow_hour"]			= (strpos(" ".$AryTmp[52],":")>0&&$AryTmp[51]>0)	? $AryTmp[52]															: "";
								$args["70"]["snow_hour"]			= is_numeric($AryTmp[53])													? number_format($AryTmp[53],	0,".", "")	: "";
							}
							//水位
							if(array_key_exists("water_level",$rec)){
								$args["40"]["water_level"]			=	is_numeric($AryTmp[33]) 				? number_format($AryTmp[33],2, ".", "")  	: "";
							}
							//最高水位
							if(array_key_exists("max_water_level",$rec)){
								$args["50"]["max_water_level"]	=	is_numeric($AryTmp[34]) 				? number_format($AryTmp[34],2, ".", "")  	: "";
								$args["55"]["max_water_level"]	=	strpos(" ".$AryTmp[35],":")>0		? $AryTmp[35]															: "";
							}
							//最低水位
							if(array_key_exists("min_water_level",$rec)){
								$args["60"]["min_water_level"]	=	is_numeric($AryTmp[36]) 				? number_format($AryTmp[36],2, ".", "")  	: "";
								$args["65"]["min_water_level"]	=	strpos(" ".$AryTmp[37],":")>0		? $AryTmp[37]															: "";
							}
							//潮位
							if(array_key_exists("tide_level",$rec)){
								$args["40"]["tide_level"]				=	is_numeric($AryTmp[38]) 				? number_format($AryTmp[38],2, ".", "")  	: "";
							}
							//最高潮位
							if(array_key_exists("max_tide_level",$rec)){
								$args["50"]["max_tide_level"]		=	is_numeric($AryTmp[39]) 				? number_format($AryTmp[39],2, ".", "")  	: "";
								$args["55"]["max_tide_level"]		=	strpos(" ".$AryTmp[40],":")>0		? $AryTmp[40]															: "";
							}
							//最低潮位
							if(array_key_exists("min_tide_level",$rec)){
								$args["60"]["min_tide_level"]		=	is_numeric($AryTmp[41]) 				? number_format($AryTmp[41],2, ".", "")  	: "";
								$args["65"]["min_tide_level"]		=	strpos(" ".$AryTmp[42],":")>0		? $AryTmp[42]															: "";
							}
							//空間線量
							if(array_key_exists("radiation",$rec)){
								$args["50"]["radiation"]		=	is_numeric($AryTmp[49]) 				? number_format($AryTmp[49],2, ".", "")  	: "";
								$args["55"]["radiation"]		=	strpos(" ".$AryTmp[50],":")>0		? $AryTmp[50]															: "";
							}

							$this->AryColSumData[ date("Ymd",$t1) ] = $args;
						}else{
							if(is_object($this->log))	$this->log->logPrint("Read Error[{$filename}]",REC_SOCK,__LINE__,__FILE__);
						}
					}else{
						if(is_object($this->log))	$this->log->logPrint("FTP GET ERROR[{$filename}]",REC_SOCK,__LINE__,__FILE__);
					}
					@unlink($localfile);

					if(!$this->monthly_mode){
						//風向頻度データ取得
						$filename		= date("Ymd",$t1)."_WD.dat";
						$localfile	= XML_DIR.$pnt_id."_".$filename;
						$remotefile	= $filename;
						@unlink($localfile);
						$ret = $this->ftp->ftp_get($localfile,$remotefile);
						if($ret){
							if($line=@file($localfile)){
								if(is_object($this->log))	$this->log->logPrint("recv:[".join("",$line)."][{$filename}]",REC_SOCK,__LINE__,__FILE__);

								$buf = trim($line[0]);
								$tmp = explode(",",$buf);

								global $dir,$dir2;

								$total	= 0;
								$AryDir	= array();
								for($i=0;$i<count($tmp);$i++){
									$AryDir[ $dir[$i] ] = $tmp[$i];
									$total += $tmp[$i];
								}

								$silent = 100;
								if($total>=0){
									for($i=0;$i<16;$i++){
										$AryDir[ $dir2[$i] ] = @floor($AryDir[ $dir2[$i] ] / $total * 1000);
										$AryDir[ $dir2[$i] ] = $AryDir[ $dir2[$i] ] / 10;
										$AryDir[ $dir2[$i] ] = number_format($AryDir[ $dir2[$i] ], 1, ".", "");
										$silent = $silent - $AryDir[ $dir2[$i] ];
									}
									$AryDir[ $dir2[16] ] = number_format($silent, 1, ".", "");
								}
								$AryTmp = array();
								for($i=0;$i<17;$i++) $AryTmp[]	= $AryDir[ $dir2[$i] ];

								$this->AryColSumData[ date("Ymd",$t1) ]["90"]["avg_ws"] = join(",",$AryTmp);;
							}else{
								if(is_object($this->log))	$this->log->logPrint("Read Error[{$filename}]",REC_SOCK,__LINE__,__FILE__);
							}
						}else{
							if(is_object($this->log))	$this->log->logPrint("FTP GET ERROR[{$filename}]",REC_SOCK,__LINE__,__FILE__);
						}
						@unlink($localfile);
					}
					$wk = date("Ymd",$t1);
				}
				$t1 += 3600;
				if($t1>time()) break;
			}
			$this->ftp->ftp_quit();
			return true;
		}

		//月報データ取得処理 [2009/02/10]
		function GetMonthlyData($pnt_id,$curTime,&$args){
			$date	= DateConvert($curTime-86400);
			$trg1	= $date["year"].$date["month"].$date["day"];
			$date	= DateConvert($curTime);
			$trg2	= $date["year"].$date["month"].$date["day"];
			$this->GetLoggerMonthlyCollection($pnt_id,$trg1,$trg2);

			$data = new Data($this->log);
			foreach($this->AryColData as $key => $val){
				$d = DateConvert2($key);
				$data->SaveData($pnt_id,"300",strtotime($d["year"]."/".$d["month"]."/".$d["day"]." 00:01"),$val,"",false);
			}
			return true;
		}
		//月報集計データ取得処理 [2009/02/10]
		function GetMonthlySumData($pnt_id,$curTime,&$args){
			$date	= DateConvert($curTime-86400);
			$trg1	= $date["year"].$date["month"];
			$date	= DateConvert($curTime);
			$trg2	= $date["year"].$date["month"];
			if(!(is_array($this->AryColSumData[$trg1])&&is_array($this->AryColSumData[$trg2]))){
				$this->GetLoggerMonthlyCollection($pnt_id,$trg1,$trg2);
			}

			$data = new Data($this->log);
			foreach($this->AryColSumData as $key => $val){
				foreach($val as $sum_no => $rec){
					$d = DateConvert2($key);
					$data->SaveData($pnt_id,"300",strtotime($d["year"]."/".$d["month"]."/01 00:01"),$rec,$sum_no,true);
				}
			}
			return true;
		}
		//月報データ手動収集（連想配列に格納したデータを取得） [2009/02/10]
		function GetMonthlyCollectionData($pnt_id,$curTime,&$args){
			$date	= DateConvert($curTime);
			$trg	= $date["year"].$date["month"].$date["day"];
			foreach($args as $fld => $val){
				$args[$fld] = $this->AryColData[$trg][$fld];
			}
			return true;
		}
		//月報集計データ手動収集（連想配列に格納したデータを取得） [2009/02/10]
		function GetMonthlyCollectionSumData($pnt_id,$curTime,&$args){
			$date	= DateConvert($curTime);
			$trg	= $date["year"].$date["month"];
			foreach($args as $sum_no => $rec){
				foreach($rec as $fld => $val){
					$args[$sum_no][$fld] = $this->AryColSumData[$trg][$sum_no][$fld];
				}
			}
			return true;
		}
		//月報データ手動収集（取得データを連想配列に格納する） [2009/02/10]
		function GetLoggerMonthlyCollection($pnt_id,$trg1,$trg2=""){
			//データ取得フィールド定義
			$data = new Data($log);
			$rec	= $data->GetDataField("300");

			if($trg2=="")	$trg2 = $trg1;

			//取得日付を強制的に変換
			$date = DateConvert2($trg1);
			$trg1	= $date["year"].$date["month"]."010010";
			$date = DateConvert2($trg2);
			$trg2	= $date["year"].$date["month"].LastDate($date["year"],$date["month"])."2400";

			if(is_object($this->log))	$this->log->logPrint("GetLoggerMonthlyCollection [{$trg1}] => [{$trg2}]",REC_SOCK,__LINE__,__FILE__);

			if(!$this->yearly_mode){
				//日報集計値を取得
				$this->monthly_mode = true;
				$this->GetLoggerDailyCollection($pnt_id,$trg1,$trg2);

				//格納用連想配列初期化
				$this->AryColData = array();

				if(!is_array($this->AryColSumData)) $this->AryColSumData = array();

				//月報データを取得
				foreach($this->AryColSumData as $trg => $ArySum){
					$args = $rec;
			    if(array_key_exists("avg_ws",$args))					$args["avg_ws"]						=	$ArySum["40"]["avg_ws"];
			    if(array_key_exists("avg_wd",$args))					$args["avg_wd"]						=	$ArySum["40"]["avg_wd"];
			    if(array_key_exists("max_ws",$args))					$args["max_ws"]						=	$ArySum["50"]["max_ws"];
			    if(array_key_exists("max_wd",$args))					$args["max_wd"]						=	$ArySum["50"]["max_wd"];
			    if(array_key_exists("avg_temp",$args))				$args["avg_temp"] 				=	$ArySum["40"]["temp"];
			    if(array_key_exists("max_temp",$args))				$args["max_temp"] 				=	$ArySum["50"]["temp"];
			    if(array_key_exists("min_temp",$args))				$args["min_temp"] 				=	$ArySum["60"]["temp"];
			    if(array_key_exists("avg_humid1",$args))			$args["avg_humid1"]				=	$ArySum["40"]["humid1"];
			    if(array_key_exists("max_humid1",$args))			$args["max_humid1"]				=	$ArySum["50"]["humid1"];
			    if(array_key_exists("min_humid1",$args))			$args["min_humid1"]				=	$ArySum["60"]["humid1"];
			    if(array_key_exists("humid2",$args))					$args["humid2"]						=	$ArySum["40"]["humid2"];
			    if(array_key_exists("press1",$args))					$args["press1"]						=	$ArySum["40"]["press1"];
			    if(array_key_exists("press2",$args))					$args["press2"]						=	$ArySum["40"]["press2"];
			    if(array_key_exists("avg_press1",$args))			$args["avg_press1"]				=	$ArySum["40"]["press1"];
			    if(array_key_exists("max_press1",$args))			$args["max_press1"]				=	$ArySum["50"]["press1"];
			    if(array_key_exists("min_press1",$args))			$args["min_press1"]				=	$ArySum["60"]["press1"];
			    if(array_key_exists("max_rain_10min",$args))	$args["max_rain_10min"]		=	$ArySum["50"]["max_rain_10min"];
			    if(array_key_exists("max_rain_hour",$args))		$args["max_rain_hour"]		=	$ArySum["50"]["rain_hour"];
			    if(array_key_exists("rain_day",$args))				$args["rain_day"]					=	$ArySum["70"]["rain_day"];
			    if(array_key_exists("sun_day",$args))					$args["sun_day"]					=	$ArySum["70"]["sun_day"];
			    if(array_key_exists("snow_level",$args))			$args["snow_level"]				=	$ArySum["50"]["snow_level"];
			    if(array_key_exists("snow_day",$args))				$args["snow_day"]					=	$ArySum["70"]["snow_hour"];
			    if(array_key_exists("avg_water_level",$args))	$args["avg_water_level"]	=	$ArySum["40"]["water_level"];
			    if(array_key_exists("max_water_level",$args))	$args["max_water_level"] 	=	$ArySum["50"]["max_water_level"];
			    if(array_key_exists("min_water_level",$args))	$args["min_water_level"] 	=	$ArySum["60"]["min_water_level"];
			    if(array_key_exists("tide_level",$args))			$args["tide_level"] 			=	$ArySum["40"]["tide_level"];
			    if(array_key_exists("max_tide_level",$args))	$args["max_tide_level"] 	=	$ArySum["50"]["max_tide_level"];
			    if(array_key_exists("min_tide_level",$args))	$args["min_tide_level"] 	=	$ArySum["60"]["min_tide_level"];
			    if(array_key_exists("radiation",$args))				$args["radiation"]				=	$ArySum["50"]["radiation"];

					$this->AryColData[ $trg ] = $args;
				}
			}

			//格納用連想配列初期化
			$this->AryColSumData = array();

			//月報集計データを取得
			if(!$this->load_flg)	$this->LoadLoggerInfo($pnt_id);
			if(!$this->CreateFTP($pnt_id)) return false;


			$isChecked = $this->yearly_mode;
			if (!$isChecked) {
				$isChecked = $this->checkWriteStatus("ST.loc");
			}
			if($isChecked){
				$date1 	= DateConvert2($trg1);
				$date2 	= DateConvert2($trg2);
				$t1 		= $date1["year"].$date1["month"];
				$t2 		= $date2["year"].$date2["month"];
				while($t1<=$t2){

					if($wk!=$t1){
						$args = array();

						//集計データ取得
						$filename		= $t1."_ST.dat";
						$localfile	= XML_DIR.$pnt_id."_".$filename;
						$remotefile	= $filename;
						@unlink($localfile);
						$ret = $this->ftp->ftp_get($localfile,$remotefile);
						if($ret){
							if($line=@file($localfile)){
								if(is_object($this->log))	$this->log->logPrint("recv:[".join("",$line)."][{$filename}]",REC_SOCK,__LINE__,__FILE__);
								$value	= str_replace("\r\n","",$line[0]);
								$tmp		= explode(",",$value);
								$buf		= $this->ClearBuf($value);
								$AryTmp	=	$this->ClearBuf2($buf);

								//平均風速
								if(array_key_exists("avg_ws",$rec)){
									$args["40"]["avg_ws"]	= is_numeric($AryTmp[1])					? number_format($AryTmp[1],	1,".", "")		: "";
									$args["50"]["avg_ws"]	= is_numeric($AryTmp[5])					? number_format($AryTmp[5],	1,".", "")		: "";
									$args["55"]["avg_ws"]	= strpos(" ".$AryTmp[7],":")>0		? $AryTmp[7]															: "";
								}
								//平均風向
								if(array_key_exists("avg_wd",$rec)){
									$args["40"]["avg_wd"]	= is_numeric($AryTmp[49])					? sprintf("%02d",$AryTmp[49]) 						: "";
									$args["50"]["avg_wd"]	= is_numeric($AryTmp[6])					? sprintf("%02d",$AryTmp[6]) 							: "";
								}
								//最大瞬間風速
								if(array_key_exists("max_ws",$rec)){
									$args["50"]["max_ws"]	= is_numeric($AryTmp[2])					? number_format($AryTmp[2],	1,".", "")		: "";
									$args["55"]["max_ws"]	= strpos(" ".$AryTmp[4],":")>0		? $AryTmp[4]															: "";
								}
								//最大瞬間風向
								if(array_key_exists("max_wd",$rec)){
									$args["50"]["max_wd"]	= is_numeric($AryTmp[3])					? sprintf("%02d",$AryTmp[3]) 							: "";
								}
								//平均気温
								if(array_key_exists("avg_temp",$rec)){
									$args["40"]["avg_temp"]	= is_numeric($AryTmp[8])					? number_format($AryTmp[8],	1,".", "")		: "";
								}
								//最高気温
								if(array_key_exists("max_temp",$rec)){
									$args["50"]["max_temp"]	= is_numeric($AryTmp[14])					? number_format($AryTmp[14],	1,".", "")	: "";
									$args["55"]["max_temp"]	= strpos(" ".$AryTmp[15],":")>0		? $AryTmp[15]															: "";
									$args["80"]["max_temp"]	= is_numeric($AryTmp[64])					? $AryTmp[64]															: "";
								}
								//最低気温
								if(array_key_exists("min_temp",$rec)){
									$args["60"]["min_temp"]	= is_numeric($AryTmp[21])					? number_format($AryTmp[21],	1,".", "")	: "";
									$args["65"]["min_temp"]	= strpos(" ".$AryTmp[22],":")>0		? $AryTmp[22]															: "";
									$args["80"]["min_temp"]	= is_numeric($AryTmp[65])					? $AryTmp[65]															: "";
								}

								//平均相対湿度
								if(array_key_exists("avg_humid1",$rec)){
									$args["40"]["avg_humid1"]	= is_numeric($AryTmp[33])				? number_format($AryTmp[33],	1,".", "")	: "";
								}
								//最高相対湿度
								if(array_key_exists("max_humid1",$rec)){
									$args["50"]["max_humid1"]	= is_numeric($AryTmp[34])				? number_format($AryTmp[34],	1,".", "")	: "";
									$args["55"]["max_humid1"]	= strpos(" ".$AryTmp[35],":")>0	? $AryTmp[35]															: "";
								}
								//最低相対湿度
								if(array_key_exists("min_humid1",$rec)){
									$args["60"]["min_humid1"]	= is_numeric($AryTmp[36])				? number_format($AryTmp[36],	1,".", "")	: "";
									$args["65"]["min_humid1"]	= strpos(" ".$AryTmp[37],":")>0	? $AryTmp[37]															: "";
								}
								//実効湿度
								if(array_key_exists("humid2",$rec)){
									$args["40"]["humid2"]	= is_numeric($AryTmp[72])				? number_format($AryTmp[72],	1,".", "")	: "";
									$args["50"]["humid2"]	= is_numeric($AryTmp[73])				? number_format($AryTmp[73],	1,".", "")	: "";
									$args["55"]["humid2"]	= strpos(" ".$AryTmp[74],":")>0	? $AryTmp[74]															: "";
									$args["60"]["humid2"]	= is_numeric($AryTmp[75])				? number_format($AryTmp[75],	1,".", "")	: "";
									$args["65"]["humid2"]	= strpos(" ".$AryTmp[76],":")>0	? $AryTmp[76]															: "";
								}
								//現地気圧
								if(array_key_exists("press1",$rec)){
									$args["40"]["press1"]	= is_numeric($AryTmp[44])				? number_format($AryTmp[44],	1,".", "")	: "";
									$args["50"]["press1"]	= is_numeric($AryTmp[45])				? number_format($AryTmp[45],	1,".", "")	: "";
									$args["55"]["press1"]	= strpos(" ".$AryTmp[46],":")>0	? $AryTmp[46]															: "";
									$args["60"]["press1"]	= is_numeric($AryTmp[47])				? number_format($AryTmp[47],	1,".", "")	: "";
									$args["65"]["press1"]	= strpos(" ".$AryTmp[48],":")>0	? $AryTmp[48]															: "";
								}
								//現地気圧
								if(array_key_exists("press2",$rec)){
									$args["40"]["press2"]	= is_numeric($AryTmp[80])				? number_format($AryTmp[80],	1,".", "")	: "";
									$args["50"]["press2"]	= is_numeric($AryTmp[81])				? number_format($AryTmp[81],	1,".", "")	: "";
									$args["55"]["press2"]	= strpos(" ".$AryTmp[82],":")>0	? $AryTmp[82]															: "";
									$args["60"]["press2"]	= is_numeric($AryTmp[83])				? number_format($AryTmp[83],	1,".", "")	: "";
									$args["65"]["press2"]	= strpos(" ".$AryTmp[84],":")>0	? $AryTmp[84]															: "";
								}
								//10分最大雨量
								if(array_key_exists("max_rain_10min",$rec)){
									$args["50"]["max_rain_10min"]	= is_numeric($AryTmp[52])													? number_format($AryTmp[52],	1,".", "")	: "";
									$args["55"]["max_rain_10min"]	= (strpos(" ".$AryTmp[53],":")>0&&$AryTmp[52]>0)	? $AryTmp[53]															: "";
								}
								//時間最大雨量
								if(array_key_exists("max_rain_hour",$rec)){
									$args["50"]["max_rain_hour"]	= is_numeric($AryTmp[50])													? number_format($AryTmp[50],	1,".", "")	: "";
									$args["55"]["max_rain_hour"]	= (strpos(" ".$AryTmp[51],":")>0&&$AryTmp[50]>0)	? $AryTmp[51]															: "";
								}
								//日積算雨量
								if(array_key_exists("rain_day",$rec)){
									$args["50"]["rain_day"]				= is_numeric($AryTmp[24])													? number_format($AryTmp[24],	1,".", "")	: "";
									$args["55"]["rain_day"]				= (strpos(" ".$AryTmp[25],"/")>0&&$AryTmp[24]>0)	? $AryTmp[25]															: "";
									$args["70"]["rain_day"]				= is_numeric($AryTmp[23])													? number_format($AryTmp[23],	1,".", "")	: "";
									$args["80"]["rain_day"]				= is_numeric($AryTmp[66])													? $AryTmp[66]															: "";
								}
								//日積算日照
								if(array_key_exists("sun_day",$rec)){
									$args["50"]["sun_day"]	= is_numeric($AryTmp[27])				? number_format($AryTmp[27],	1,".", "")	: "";
									$args["55"]["sun_day"]	= strpos(" ".$AryTmp[28],"/")>0	? $AryTmp[28]															: "";
									$args["60"]["sun_day"]	= is_numeric($AryTmp[29])				? number_format($AryTmp[29],	1,".", "")	: "";
									$args["65"]["sun_day"]	= strpos(" ".$AryTmp[30],"/")>0	? $AryTmp[30]															: "";
									$args["70"]["sun_day"]	= is_numeric($AryTmp[26])				? number_format($AryTmp[26],	1,".", "")	: "";
									$args["80"]["sun_day"]	= is_numeric($AryTmp[77])				? number_format($AryTmp[77],	0,".", "")	: "";
								}
								//積雪深
								if(array_key_exists("snow_level",$rec)){
									$args["50"]["snow_level"]	= is_numeric($AryTmp[31])				? number_format($AryTmp[31],	0,".", "")	: "";
									$args["55"]["snow_level"]	= strpos(" ".$AryTmp[32],"/")>0	? $AryTmp[32]															: "";
								}
								//降雪量
								if(array_key_exists("snow_day",$rec)){
									$args["50"]["snow_day"]			= is_numeric($AryTmp[80])													? number_format($AryTmp[80],	0,".", "")	: "";
									$args["55"]["snow_day"]			= (strpos(" ".$AryTmp[81],"/")>0&&$AryTmp[80]>0)	? $AryTmp[81]															: "";
									$args["70"]["snow_day"]			= is_numeric($AryTmp[82])													? number_format($AryTmp[82],	0,".", "")	: "";
								}
								//水位
								if(array_key_exists("avg_water_level",$rec)){
									$args["40"]["avg_water_level"]			=	is_numeric($AryTmp[54]) 				? number_format($AryTmp[54],2, ".", "")  	: "";
								}
								//最高水位
								if(array_key_exists("max_water_level",$rec)){
									$args["50"]["max_water_level"]	=	is_numeric($AryTmp[55]) 				? number_format($AryTmp[55],2, ".", "")  	: "";
									$args["55"]["max_water_level"]	=	strpos(" ".$AryTmp[56],":")>0		? $AryTmp[56]															: "";
								}
								//最低水位
								if(array_key_exists("min_water_level",$rec)){
									$args["60"]["min_water_level"]	=	is_numeric($AryTmp[57]) 				? number_format($AryTmp[57],2, ".", "")  	: "";
									$args["65"]["min_water_level"]	=	strpos(" ".$AryTmp[58],":")>0		? $AryTmp[58]															: "";
								}
								//潮位
								if(array_key_exists("tide_level",$rec)){
								$args["40"]["tide_level"]				=	is_numeric($AryTmp[59]) 				? number_format($AryTmp[59],2, ".", "")  	: "";
								}
								//最高潮位
								if(array_key_exists("max_tide_level",$rec)){
									$args["50"]["max_tide_level"]		=	is_numeric($AryTmp[60]) 				? number_format($AryTmp[60],2, ".", "")  	: "";
									$args["55"]["max_tide_level"]		=	strpos(" ".$AryTmp[61],":")>0		? $AryTmp[61]															: "";
								}
								//最低潮位
								if(array_key_exists("min_tide_level",$rec)){
									$args["60"]["min_tide_level"]		=	is_numeric($AryTmp[62]) 				? number_format($AryTmp[62],2, ".", "")  	: "";
									$args["65"]["min_tide_level"]		=	strpos(" ".$AryTmp[63],":")>0		? $AryTmp[63]															: "";
								}
								//空間線量
								if(array_key_exists("radiation",$rec)){
									$args["50"]["radiation"]		=	is_numeric($AryTmp[78]) 				? number_format($AryTmp[78],2, ".", "")  	: "";
									$args["55"]["radiation"]		=	strpos(" ".$AryTmp[79],"/")>0		? $AryTmp[79]															: "";
								}
								$this->AryColSumData[ $t1 ] = $args;
								if(is_object($this->log))	$this->log->logPrint("Read Ok[{$filename}]",REC_SOCK,__LINE__,__FILE__);
							}else{
								if(is_object($this->log))	$this->log->logPrint("Read Error[{$filename}]",REC_SOCK,__LINE__,__FILE__);
							}
						}else{
							if(is_object($this->log))	$this->log->logPrint("FTP GET ERROR[{$filename}]",REC_SOCK,__LINE__,__FILE__);
						}
						@unlink($localfile);

						if(!$this->yearly_mode){
							//風向頻度データ取得
							$filename		= $t1."_WD.dat";
							$localfile	= XML_DIR.$pnt_id."_".$filename;
							$remotefile	= $filename;
							@unlink($localfile);
							$ret = $this->ftp->ftp_get($localfile,$remotefile);
							if($ret){
								if($line=@file($localfile)){
									if(is_object($this->log))	$this->log->logPrint("recv:[".join("",$line)."][{$filename}]",REC_SOCK,__LINE__,__FILE__);

									$buf = trim($line[0]);
									$tmp = explode(",",$buf);

									global $dir,$dir2;

									$total	= 0;
									$AryDir	= array();
									for($i=0;$i<count($tmp);$i++){
										$AryDir[ $dir[$i] ] = $tmp[$i];
										$total += $tmp[$i];
									}

									$silent = 100;
									if($total>=0){
										for($i=0;$i<16;$i++){
											$AryDir[ $dir2[$i] ] = @floor($AryDir[ $dir2[$i] ] / $total * 1000);
											$AryDir[ $dir2[$i] ] = $AryDir[ $dir2[$i] ] / 10;
											$AryDir[ $dir2[$i] ] = number_format($AryDir[ $dir2[$i] ], 1, ".", "");
											$silent = $silent - $AryDir[ $dir2[$i] ];
										}
										$AryDir[ $dir2[16] ] = number_format($silent, 1, ".", "");
									}
									$AryTmp = array();
									for($i=0;$i<17;$i++) $AryTmp[]	= $AryDir[ $dir2[$i] ];

									$this->AryColSumData[ $t1 ]["90"]["avg_ws"] = join(",",$AryTmp);;
								}else{
									if(is_object($this->log))	$this->log->logPrint("Read Error[{$filename}]",REC_SOCK,__LINE__,__FILE__);
								}
							}else{
								if(is_object($this->log))	$this->log->logPrint("FTP GET ERROR[{$filename}]",REC_SOCK,__LINE__,__FILE__);
							}
						}

						$wk = $t1;
					}
					$date		= DateConvert2($t1);
					$year 	= $date["year"];
					$month	= sprintf("%02d",$date["month"]+1);
					if($month=="13"){
						$month	= "01";
						$year 	= $date["year"] + 1;
					}
					$t1 = $year.$month;

					if($t1>date("Ym")) break;
				}
			}
			$this->ftp->ftp_quit();
			return true;
		}

///////////////////////////////////////////////////////////////
		//月報データ取得処理 [2009/02/10]
		function GetYearlyData($pnt_id,$curTime,&$args){
			$date	= DateConvert($curTime-86400);
			$trg1	= $date["year"];
			$date	= DateConvert($curTime);
			$trg2	= $date["year"];
			$this->GetLoggerYearlyCollection($pnt_id,$trg1,$trg2);

			$data = new Data($this->log);
			foreach($this->AryColData as $key => $val){
				$d = DateConvert2($key);
				$data->SaveData($pnt_id,"400",strtotime($d["year"]."/".$d["month"]."/01 00:01"),$val,"",false);
			}
			return true;
		}
		//月報集計データ取得処理 [2009/02/10]
		function GetYearlySumData($pnt_id,$curTime,&$args){
			$date	= DateConvert($curTime-86400);
			$trg1	= $date["year"];
			$date	= DateConvert($curTime);
			$trg2	= $date["year"];
			if(!(is_array($this->AryColSumData[$trg1])&&is_array($this->AryColSumData[$trg2]))){
				$this->GetLoggerYearlyCollection($pnt_id,$trg1,$trg2);
			}

			$data = new Data($this->log);
			foreach($this->AryColSumData as $key => $val){
				foreach($val as $sum_no => $rec){
					$d = DateConvert2($key);
					$data->SaveData($pnt_id,"400",strtotime($d["year"]."/01/01 00:01"),$rec,$sum_no,true);
				}
			}
			return true;
		}
		//月報データ手動収集（連想配列に格納したデータを取得） [2009/02/10]
		function GetYearlyCollectionData($pnt_id,$curTime,&$args){
			$date	= DateConvert($curTime);
			$trg	= $date["year"].$date["month"];
			foreach($args as $fld => $val){
				$args[$fld] = $this->AryColData[$trg][$fld];
			}
			return true;
		}
		//月報集計データ手動収集（連想配列に格納したデータを取得） [2009/02/10]
		function GetYearlyCollectionSumData($pnt_id,$curTime,&$args){
			$date	= DateConvert($curTime);
			$trg	= $date["year"];
			foreach($args as $sum_no => $rec){
				foreach($rec as $fld => $val){
					$args[$sum_no][$fld] = $this->AryColSumData[$trg][$sum_no][$fld];
				}
			}
			return true;
		}
		//月報データ手動収集（取得データを連想配列に格納する） [2009/02/10]
		function GetLoggerYearlyCollection($pnt_id,$trg1,$trg2=""){
			//データ取得フィールド定義
			$data = new Data($log);
			$rec	= $data->GetDataField("400");

			if(!$this->checkWriteStatus("ST.loc")) return false;

			if($trg2=="")	$trg2 = $trg1;

			//取得日付を強制的に変換
			$date = DateConvert2($trg1);
			$trg1	= $date["year"]."01010010";
			$date = DateConvert2($trg2);
			$trg2	= $date["year"]."12312400";

			if(is_object($this->log))	$this->log->logPrint("GetLoggerYearlyCollection [{$trg1}] => [{$trg2}]",REC_SOCK,__LINE__,__FILE__);

			//日報集計値を取得
			$this->yearly_mode = true;
			$this->GetLoggerMonthlyCollection($pnt_id,$trg1,$trg2);

			//格納用連想配列初期化
			$this->AryColData = array();

			//月報データを取得
			foreach($this->AryColSumData as $trg => $ArySum){
				$args = $rec;
		    if(array_key_exists("avg_ws",$args))					$args["avg_ws"]						=	$ArySum["40"]["avg_ws"];
		    if(array_key_exists("avg_wd",$args))					$args["avg_wd"]						=	$ArySum["40"]["avg_wd"];
		    if(array_key_exists("max_ws",$args))					$args["max_ws"]						=	$ArySum["50"]["max_ws"];
		    if(array_key_exists("max_wd",$args))					$args["max_wd"]						=	$ArySum["50"]["max_wd"];
		    if(array_key_exists("avg_temp",$args))				$args["avg_temp"] 				=	$ArySum["40"]["avg_temp"];
		    if(array_key_exists("max_temp",$args))				$args["max_temp"] 				=	$ArySum["50"]["max_temp"];
		    if(array_key_exists("min_temp",$args))				$args["min_temp"] 				=	$ArySum["60"]["min_temp"];
		    if(array_key_exists("avg_humid1",$args))			$args["avg_humid1"]				=	$ArySum["40"]["avg_humid1"];
		    if(array_key_exists("max_humid1",$args))			$args["max_humid1"]				=	$ArySum["50"]["max_humid1"];
		    if(array_key_exists("min_humid1",$args))			$args["min_humid1"]				=	$ArySum["60"]["min_humid1"];
		    if(array_key_exists("humid2",$args))					$args["humid2"]						=	$ArySum["40"]["humid2"];
		    if(array_key_exists("press1",$args))					$args["press1"]						=	$ArySum["40"]["press1"];
		    if(array_key_exists("press2",$args))					$args["press2"]						=	$ArySum["40"]["press2"];
		    if(array_key_exists("avg_press1",$args))			$args["avg_press1"]				=	$ArySum["40"]["press1"];
		    if(array_key_exists("max_press1",$args))			$args["max_press1"]				=	$ArySum["50"]["press1"];
		    if(array_key_exists("min_press1",$args))			$args["min_press1"]				=	$ArySum["60"]["press1"];
		    if(array_key_exists("max_rain_10min",$args))	$args["max_rain_10min"]		=	$ArySum["50"]["max_rain_10min"];
		    if(array_key_exists("max_rain_hour",$args))		$args["max_rain_hour"]		=	$ArySum["50"]["max_rain_hour"];
		    if(array_key_exists("max_rain_day",$args))		$args["max_rain_day"]			=	$ArySum["50"]["rain_day"];
		    if(array_key_exists("rain_month",$args))			$args["rain_month"]				=	$ArySum["70"]["rain_day"];
		    if(array_key_exists("sun_month",$args))				$args["sun_month"]				=	$ArySum["70"]["sun_day"];
		    if(array_key_exists("snow_level",$args))			$args["snow_level"]				=	$ArySum["50"]["snow_level"];
		    if(array_key_exists("snow_month",$args))			$args["snow_month"]				=	$ArySum["70"]["snow_day"];
		    if(array_key_exists("max_snow_day",$args))		$args["max_snow_day"]			=	$ArySum["50"]["snow_day"];
		    if(array_key_exists("avg_water_level",$args))	$args["avg_water_level"]	=	$ArySum["40"]["avg_water_level"];
		    if(array_key_exists("max_water_level",$args))	$args["max_water_level"] 	=	$ArySum["50"]["max_water_level"];
		    if(array_key_exists("min_water_level",$args))	$args["min_water_level"] 	=	$ArySum["60"]["min_water_level"];
		    if(array_key_exists("tide_level",$args))			$args["tide_level"] 			=	$ArySum["40"]["tide_level"];
		    if(array_key_exists("max_tide_level",$args))	$args["max_tide_level"] 	=	$ArySum["50"]["max_tide_level"];
		    if(array_key_exists("min_tide_level",$args))	$args["min_tide_level"] 	=	$ArySum["60"]["min_tide_level"];
		    if(array_key_exists("radiation",$args))				$args["radiation"]				=	$ArySum["50"]["radiation"];
				$this->AryColData[ $trg ] = $args;
			}

			//格納用連想配列初期化
			$this->AryColSumData = array();

			//月報集計データを取得
			if(!$this->load_flg)	$this->LoadLoggerInfo($pnt_id);
			if(!$this->CreateFTP($pnt_id)) return false;

			$date1 	= DateConvert2($trg1);
			$date2 	= DateConvert2($trg2);
			$t1 		= $date1["year"];
			$t2 		= $date2["year"];

			while($t1<=$t2){

				if($wk!=$t1){

//					if($this->checkWriteStatus("ST.loc")){

						$args = array();

						//集計データ取得
						$filename		= $t1."_ST.dat";
						$localfile	= XML_DIR.$pnt_id."_".$filename;
						$remotefile	= $filename;
						@unlink($localfile);
						$ret = $this->ftp->ftp_get($localfile,$remotefile);
						if($ret){
							if($line=@file($localfile)){
								if(is_object($this->log))	$this->log->logPrint("recv:[".join("",$line)."][{$filename}]",REC_SOCK,__LINE__,__FILE__);
								$value	= str_replace("\r\n","",$line[0]);
								$tmp		= explode(",",$value);
								$buf		= $this->ClearBuf($value);
								$AryTmp	=	$this->ClearBuf2($buf);

								//平均風速
								if(array_key_exists("avg_ws",$rec)){
									$args["40"]["avg_ws"]	= is_numeric($AryTmp[1])					? number_format($AryTmp[1],	1,".", "")		: "";
									$args["50"]["avg_ws"]	= is_numeric($AryTmp[5])					? number_format($AryTmp[5],	1,".", "")		: "";
									$args["55"]["avg_ws"]	= strpos(" ".$AryTmp[7],":")>0		? $AryTmp[7]															: "";
								}
								//平均風向
								if(array_key_exists("avg_wd",$rec)){
									$args["40"]["avg_wd"]	= is_numeric($AryTmp[49])					? sprintf("%02d",$AryTmp[49]) 						: "";
									$args["50"]["avg_wd"]	= is_numeric($AryTmp[6])					? sprintf("%02d",$AryTmp[6]) 							: "";
								}
								//最大瞬間風速
								if(array_key_exists("max_ws",$rec)){
									$args["50"]["max_ws"]	= is_numeric($AryTmp[2])					? number_format($AryTmp[2],	1,".", "")		: "";
									$args["55"]["max_ws"]	= strpos(" ".$AryTmp[4],":")>0		? $AryTmp[4]															: "";
								}
								//最大瞬間風向
								if(array_key_exists("max_wd",$rec)){
									$args["50"]["max_wd"]	= is_numeric($AryTmp[3])					? sprintf("%02d",$AryTmp[3]) 							: "";
								}

								//平均気温
								if(array_key_exists("avg_temp",$rec)){
									$args["40"]["avg_temp"]	= is_numeric($AryTmp[8])					? number_format($AryTmp[8],	1,".", "")		: "";
								}
								//最高気温
								if(array_key_exists("max_temp",$rec)){
									$args["50"]["max_temp"]	= is_numeric($AryTmp[14])					? number_format($AryTmp[14],	1,".", "")	: "";
									$args["55"]["max_temp"]	= strpos(" ".$AryTmp[15],":")>0		? $AryTmp[15]															: "";
									$args["80"]["max_temp"]	= is_numeric($AryTmp[64])					? $AryTmp[64]															: "";
								}
								//最低気温
								if(array_key_exists("min_temp",$rec)){
									$args["60"]["min_temp"]	= is_numeric($AryTmp[21])					? number_format($AryTmp[21],	1,".", "")	: "";
									$args["65"]["min_temp"]	= strpos(" ".$AryTmp[22],":")>0		? $AryTmp[22]															: "";
									$args["80"]["min_temp"]	= is_numeric($AryTmp[65])					? $AryTmp[65]															: "";
								}
								//平均相対湿度
								if(array_key_exists("avg_humid1",$rec)){
									$args["40"]["avg_humid1"]	= is_numeric($AryTmp[33])				? number_format($AryTmp[33],	1,".", "")	: "";
								}
								//最高相対湿度
								if(array_key_exists("max_humid1",$rec)){
									$args["50"]["max_humid1"]	= is_numeric($AryTmp[34])				? number_format($AryTmp[34],	1,".", "")	: "";
									$args["55"]["max_humid1"]	= strpos(" ".$AryTmp[35],":")>0	? $AryTmp[35]															: "";
								}
								//最低相対湿度
								if(array_key_exists("min_humid1",$rec)){
									$args["60"]["min_humid1"]	= is_numeric($AryTmp[36])				? number_format($AryTmp[36],	1,".", "")	: "";
									$args["65"]["min_humid1"]	= strpos(" ".$AryTmp[37],":")>0	? $AryTmp[37]															: "";
								}
								//実効湿度
								if(array_key_exists("humid2",$rec)){
									$args["40"]["humid2"]	= is_numeric($AryTmp[74])				? number_format($AryTmp[74],	1,".", "")	: "";
									$args["50"]["humid2"]	= is_numeric($AryTmp[75])				? number_format($AryTmp[75],	1,".", "")	: "";
									$args["55"]["humid2"]	= strpos(" ".$AryTmp[76],":")>0	? $AryTmp[76]															: "";
									$args["60"]["humid2"]	= is_numeric($AryTmp[77])				? number_format($AryTmp[77],	1,".", "")	: "";
									$args["65"]["humid2"]	= strpos(" ".$AryTmp[78],":")>0	? $AryTmp[78]															: "";
								}
								//現地気圧
								if(array_key_exists("press1",$rec)){
									$args["40"]["press1"]	= is_numeric($AryTmp[44])				? number_format($AryTmp[44],	1,".", "")	: "";
									$args["50"]["press1"]	= is_numeric($AryTmp[45])				? number_format($AryTmp[45],	1,".", "")	: "";
									$args["55"]["press1"]	= strpos(" ".$AryTmp[46],":")>0	? $AryTmp[46]															: "";
									$args["60"]["press1"]	= is_numeric($AryTmp[47])				? number_format($AryTmp[47],	1,".", "")	: "";
									$args["65"]["press1"]	= strpos(" ".$AryTmp[48],":")>0	? $AryTmp[48]															: "";
								}
								//現地気圧
								if(array_key_exists("press2",$rec)){
									$args["40"]["press2"]	= is_numeric($AryTmp[80])				? number_format($AryTmp[80],	1,".", "")	: "";
									$args["50"]["press2"]	= is_numeric($AryTmp[81])				? number_format($AryTmp[81],	1,".", "")	: "";
									$args["55"]["press2"]	= strpos(" ".$AryTmp[82],":")>0	? $AryTmp[82]															: "";
									$args["60"]["press2"]	= is_numeric($AryTmp[83])				? number_format($AryTmp[83],	1,".", "")	: "";
									$args["65"]["press2"]	= strpos(" ".$AryTmp[84],":")>0	? $AryTmp[84]															: "";
								}
								//10分最大雨量
								if(array_key_exists("max_rain_10min",$rec)){
									$args["50"]["max_rain_10min"]	= is_numeric($AryTmp[52])													? number_format($AryTmp[52],	1,".", "")	: "";
									$args["55"]["max_rain_10min"]	= (strpos(" ".$AryTmp[53],":")>0&&$AryTmp[52]>0)	? $AryTmp[53]															: "";
								}
								//時間最大雨量
								if(array_key_exists("max_rain_hour",$rec)){
									$args["50"]["max_rain_hour"]	= is_numeric($AryTmp[50])													? number_format($AryTmp[50],	1,".", "")	: "";
									$args["55"]["max_rain_hour"]	= (strpos(" ".$AryTmp[51],":")>0&&$AryTmp[50]>0)	? $AryTmp[51]															: "";
								}
								//日最大雨量
								if(array_key_exists("max_rain_day",$rec)){
									$args["50"]["max_rain_day"]		= is_numeric($AryTmp[24])													? number_format($AryTmp[24],	1,".", "")	: "";
									$args["55"]["max_rain_day"]		= (strpos(" ".$AryTmp[25],"/")>0&&$AryTmp[24]>0)	? $AryTmp[25]															: "";
								}
								//月積算雨量
								if(array_key_exists("rain_month",$rec)){
									$args["50"]["rain_month"]			= is_numeric($AryTmp[67])													? number_format($AryTmp[67],	1,".", "")	: "";
									$args["55"]["rain_month"]			= $AryTmp[67]>0																		? $AryTmp[68]															: "";
									$args["70"]["rain_month"]			= is_numeric($AryTmp[23])													? number_format($AryTmp[23],	1,".", "")	: "";
									$args["80"]["rain_month"]			= is_numeric($AryTmp[66])													? $AryTmp[66]															: "";
								}
								//日照
								if(array_key_exists("sun_month",$rec)){
									$args["50"]["sun_month"]	= is_numeric($AryTmp[27])				? number_format($AryTmp[27],	0,".", "")	: "";
									$args["55"]["sun_month"]	= strpos(" ".$AryTmp[28],"/")>0	? $AryTmp[28]															: "";
									$args["60"]["sun_month"]	= is_numeric($AryTmp[29])				? number_format($AryTmp[29],	0,".", "")	: "";
									$args["65"]["sun_month"]	= strpos(" ".$AryTmp[30],"/")>0	? $AryTmp[30]															: "";
									$args["70"]["sun_month"]	= is_numeric($AryTmp[26])				? number_format($AryTmp[26],	0,".", "")	: "";
									$args["80"]["sun_month"]	= is_numeric($AryTmp[79])				? number_format($AryTmp[79],	0,".", "")	: "";
								}
								//積雪深
								if(array_key_exists("snow_level",$rec)){
	//								$args["40"]["snow_level"]	= is_numeric($AryTmp[44])				? number_format($AryTmp[44],	1,".", "")	: "";
									$args["50"]["snow_level"]	= is_numeric($AryTmp[31])				? number_format($AryTmp[31],	0,".", "")	: "";
									$args["55"]["snow_level"]	= strpos(" ".$AryTmp[32],":")>0	? $AryTmp[32]															: "";
	//								$args["60"]["snow_level"]	= is_numeric($AryTmp[47])				? number_format($AryTmp[47],	1,".", "")	: "";
	//								$args["65"]["snow_level"]	= strpos(" ".$AryTmp[48],":")>0	? $AryTmp[48]															: "";
								//降雪量
								if(array_key_exists("snow_month",$rec)){
									$args["50"]["snow_month"]			= is_numeric($AryTmp[82])													? number_format($AryTmp[82],	0,".", "")	: "";
									$args["55"]["snow_month"]			= (is_numeric($AryTmp[83])>0&&$AryTmp[82]>0)	? $AryTmp[83]															: "";
									$args["70"]["snow_month"]			= is_numeric($AryTmp[84])													? number_format($AryTmp[84],	0,".", "")	: "";
								}
								if(array_key_exists("max_snow_day",$rec)){
									$args["50"]["max_snow_day"]	= is_numeric($AryTmp[85])				? number_format($AryTmp[85],	0,".", "")	: "";
									$args["55"]["max_snow_day"]	= strpos(" ".$AryTmp[86],"/")>0	? $AryTmp[86]															: "";
								}							}
								//水位
								if(array_key_exists("avg_water_level",$rec)){
									$args["40"]["avg_water_level"]			=	is_numeric($AryTmp[54]) 				? number_format($AryTmp[54],2, ".", "")  	: "";
								}
								//最高水位
								if(array_key_exists("max_water_level",$rec)){
									$args["50"]["max_water_level"]	=	is_numeric($AryTmp[55]) 				? number_format($AryTmp[55],2, ".", "")  	: "";
									$args["55"]["max_water_level"]	=	strpos(" ".$AryTmp[56],":")>0		? $AryTmp[56]															: "";
								}
								//最低水位
								if(array_key_exists("min_water_level",$rec)){
									$args["60"]["min_water_level"]	=	is_numeric($AryTmp[57]) 				? number_format($AryTmp[57],2, ".", "")  	: "";
									$args["65"]["min_water_level"]	=	strpos(" ".$AryTmp[58],":")>0		? $AryTmp[58]															: "";
								}
								//潮位
								if(array_key_exists("tide_level",$rec)){
									$args["40"]["tide_level"]				=	is_numeric($AryTmp[59]) 				? number_format($AryTmp[59],2, ".", "")  	: "";
								}
								//最高潮位
								if(array_key_exists("max_tide_level",$rec)){
									$args["50"]["max_tide_level"]		=	is_numeric($AryTmp[60]) 				? number_format($AryTmp[60],2, ".", "")  	: "";
									$args["55"]["max_tide_level"]		=	strpos(" ".$AryTmp[61],":")>0		? $AryTmp[61]															: "";
								}
								//最低潮位
								if(array_key_exists("min_tide_level",$rec)){
									$args["60"]["min_tide_level"]		=	is_numeric($AryTmp[62]) 				? number_format($AryTmp[62],2, ".", "")  	: "";
									$args["65"]["min_tide_level"]		=	strpos(" ".$AryTmp[63],":")>0		? $AryTmp[63]															: "";
								}
								//空間線量
								if(array_key_exists("radiation",$rec)){
									$args["50"]["radiation"]		=	is_numeric($AryTmp[80]) 				? number_format($AryTmp[80],2, ".", "")  	: "";
									$args["55"]["radiation"]		=	strpos(" ".$AryTmp[81],"/")>0		? $AryTmp[81]															: "";
								}
								$this->AryColSumData[ $t1 ] = $args;
							}else{
								if(is_object($this->log))	$this->log->logPrint("Read Error[{$filename}]",REC_SOCK,__LINE__,__FILE__);
							}
						}else{
							if(is_object($this->log))	$this->log->logPrint("FTP GET ERROR[{$filename}]",REC_SOCK,__LINE__,__FILE__);
						}
						@unlink($localfile);
//					}

					//風向頻度データ取得
					$filename		= $t1."_WD.dat";
					$localfile	= XML_DIR.$pnt_id."_".$filename;
					$remotefile	= $filename;
					@unlink($localfile);
					$ret = $this->ftp->ftp_get($localfile,$remotefile);
					if($ret){
						if($line=@file($localfile)){
							if(is_object($this->log))	$this->log->logPrint("recv:[".join("",$line)."][{$filename}]",REC_SOCK,__LINE__,__FILE__);

							$buf = trim($line[0]);
							$tmp = explode(",",$buf);

							global $dir,$dir2;

							$total	= 0;
							$AryDir	= array();
							for($i=0;$i<count($tmp);$i++){
								$AryDir[ $dir[$i] ] = $tmp[$i];
								$total += $tmp[$i];
							}

							$silent = 100;
							if($total>=0){
								for($i=0;$i<16;$i++){
									$AryDir[ $dir2[$i] ] = @floor($AryDir[ $dir2[$i] ] / $total * 1000);
									$AryDir[ $dir2[$i] ] = $AryDir[ $dir2[$i] ] / 10;
									$AryDir[ $dir2[$i] ] = number_format($AryDir[ $dir2[$i] ], 1, ".", "");
									$silent = $silent - $AryDir[ $dir2[$i] ];
								}
								$AryDir[ $dir2[16] ] = number_format($silent, 1, ".", "");
							}
							$AryTmp = array();
							for($i=0;$i<17;$i++) $AryTmp[]	= $AryDir[ $dir2[$i] ];

							$this->AryColSumData[ $t1 ]["90"]["avg_ws"] = join(",",$AryTmp);;
						}else{
							if(is_object($this->log))	$this->log->logPrint("Read Error[{$filename}]",REC_SOCK,__LINE__,__FILE__);
						}
					}else{
						if(is_object($this->log))	$this->log->logPrint("FTP GET ERROR[{$filename}]",REC_SOCK,__LINE__,__FILE__);
					}
					@unlink($localfile);

					$wk = $t1;
				}
				$t1++;
			}
			$this->ftp->ftp_quit();
			return true;
		}
/////////////////////////////////////////////////////////////////
	}
?>