<?php
/**
 * 自動出動インターフェイスでALARM.CSV、ALARM.FLGを出力する為のクラスです。
 * 
 * ネーミングが変だけどとりあえずそのまま利用（一部改変）
 */
class GenzaiIf {
	private $SETTINGS_GENZAI;

	private $curTime;

	function getSuffix(){
		$WARNING_SUFFIX = array(
			"1" => "注意報",
			"2" => "警報",
			"3" => "特別警報"
		);
		return $WARNING_SUFFIX;
	}

	function getPath($name="TMP_ALARM_PLACE_PATH"){
		if($name=="TMP_ALARM_PLACE_PATH"){
			return ROOT_DIR."kanri/warmst/ALARM_PLACE.CSV";
		}
		if($name=="ALARM_PLACE_PATH"){
			return FTP_DIR."ALARM_PLACE.CSV";
		}
		if($name=="ALARM_NAME_PATH"){
			return FTP_DIR."ALARM_NAME.CSV";
		}
		if($name=="BEFORE_ALARM_CSV"){// 使ってないけどな！・・大〇氏？？
			return ROOT_DIR."kanri/warmst/ALARM.CSV";
		}
		return ROOT_DIR."kanri/warmst/ALARM_PLACE.CSV";
	}

	public function __construct($curTime){
		$this->SETTINGS_GENZAI = array(
			array(
				"name" => "date",
				"function" => "getDate"
			),
			array(
				"name" => "time",
				"function" => "getTime"
			),
			array(
				"name" => "mode",
				"function" => "getMode"
			),
			array(
				"name" => "warning_setting",
				"index" => array(0, 2)
			),
			array(
				"name" => "warning",
				"index" => array(1, 2)
			)
		);

		$this->curTime = $curTime;
	}

	// ※豊田のソースから持ってきたが、千葉では未使用なので未調整だし、調整する気にもならん
	public function putCSV(){
		$log = new Recorder("if");

		$_points = new Points;
		$points = $_points->GetPoints();

		foreach($points as $point){
			$year = date('Y', $this->curTime);
			$month = date('m', $this->curTime);
			preg_match('/0([0-9]{2})/', $point["pnt_id"], $m);
			$pnt = $m[1];

			#閾値判断のAPIを叩けば楽ちんなのだ
			$_warnings = file("http://127.0.0.1/".(defined("AJUST_ROOT_DIR") ? AJUST_ROOT_DIR : "")."keiho_threshold_xml.php?pnt={$point["pnt_id"]}&trg=$year$month&kind=csv");
			array_shift($_warnings);

			$month = sprintf("%02d", --$month);
			$lines = file("http://127.0.0.1/".(defined("AJUST_ROOT_DIR") ? AJUST_ROOT_DIR : "")."keiho_threshold_xml.php?pnt={$point["pnt_id"]}&trg=$year$month&kind=csv");
			array_shift($lines);
			$_warnings = array_merge($_warnings, $lines);
			$month = sprintf("%02d", --$month);
			$lines = file("http://127.0.0.1/".(defined("AJUST_ROOT_DIR") ? AJUST_ROOT_DIR : "")."keiho_threshold_xml.php?pnt={$point["pnt_id"]}&trg=$year$month&kind=csv");
			array_shift($lines);
			$_warnings = array_merge($_warnings, $lines);

			#警報解除されていないものを選定。閾値判断のAPIは履歴も残している
			$warnings = array();
			for($i = 0 ; $i <= count($_warnings) ; $i += 2){
				$warningToken = explode(',', $_warnings[$i]);

				if($warningToken[3] === ""){
					$warnings[] = array($warningToken, explode(',', $_warnings[$i+1]));
				}
			}

			#値を書き込み
			$save = '';
			foreach($warnings as $warning){
				$data = array();

				#設定に従って文字列を処理
				foreach($this->SETTINGS_GENZAI as $settings){
					if(isset($settings["function"])){
						$data[] = $this->{$settings["function"]}($warning);
					}else{
						$data[] = $this->get($warning, $settings["index"][0], $settings["index"][1]);
					}
				}

//				$save .= mb_convert_encoding(implode(',', $data)."\n", 'SJIS', 'UTF-8');
				$save .= mb_convert_encoding(implode(',', $data)."\n", 'SJIS', 'EUC-JP');
			}

			#前回の状態と変化がある場合、書き込みを行う。
			$old = @file_get_contents(FTP_DIR."ALARM{$pnt}.CSV");
			if($old===false) $old = "X0P7yCclhHmzg7";// ファイルが存在しない場合かつ出力データ無い場合の対策
			if(trim($save) != trim($old)){
				touch(FTP_DIR."ALARM{$pnt}.FLG");
				$log->logPrint("touch(".FTP_DIR."ALARM{$pnt}.FLG".")", REC_SOCK,__LINE__,__FILE__);

				file_put_contents(FTP_DIR."ALARM{$pnt}.CSV", $save);
				$log->logPrint("file_put_contents(".FTP_DIR."ALARM{$pnt}.CSV".", $save);", REC_SOCK,__LINE__,__FILE__);

				$unlinkReturn = unlink(FTP_DIR."ALARM{$pnt}.FLG");
				$log->logPrint("unlink(".FTP_DIR."ALARM{$pnt}.FLG".")", REC_SOCK,__LINE__,__FILE__);
				$log->logPrint("unlink return:[$unlinkReturn]", REC_SOCK,__LINE__,__FILE__);
			}else{
				$log->logPrint(FTP_DIR."ALARM{$pnt}.CSV same content", REC_SOCK,__LINE__,__FILE__);
			}
		}

		$this->put_alarm_csv();
	}

	function put_alarm_csv(){
		$log = new Recorder("if");

		$_points = new Points;
		$points = $_points->GetPoints();

		$_alert = new Alert;

		$write = "";
		foreach($points as $point){
			$alerts = $_alert->GetWarning($point["pnt_id"]);

			if($alerts == false) continue;

			foreach($alerts as $alert){
				$area_id = $this->to_alarm_place($point["pnt_id"]);
				$day = date("Ymd", strtotime($alert["date"]));
				$time = date("H:i", strtotime($alert["date"]));

				$write .= $area_id.",".$day.",".$time.",".sprintf("%02d", $alert["war_no"]).",\n";
			}
		}

		#前回の状態と変化がある場合、書き込みを行う。
		$old = @file_get_contents(FTP_DIR."ALARM.CSV");
		if($old===false) $old = "X0P7yCclhHmzg7";// ファイルが存在しない場合かつ出力データ無い場合の対策
		if(trim($write) != trim($old)){
			touch(FTP_DIR.'ALARM.FLG');
			$log->logPrint("touch(".FTP_DIR.'ALARM.FLG'.")", REC_SOCK,__LINE__,__FILE__);

			file_put_contents(FTP_DIR."ALARM.CSV", mb_convert_encoding($write, "SJIS", "EUC-JP"));
			$log->logPrint("file_put_contents(".FTP_DIR."ALARM.CSV".")", REC_SOCK,__LINE__,__FILE__);

			$unlinkReturn = unlink(FTP_DIR.'ALARM.FLG');
			$log->logPrint("unlink(".FTP_DIR."ALARM.FLG".")", REC_SOCK,__LINE__,__FILE__);
			$log->logPrint("unlink return:[$unlinkReturn]", REC_SOCK,__LINE__,__FILE__);
		}else{
			$log->logPrint(FTP_DIR."ALARM.CSV same content", REC_SOCK,__LINE__,__FILE__);
		}

		#1年以上経過したファイルは削除するようにする
		if(file_exists(FTP_DIR."ALARM.CSV") && 31536000 < time() - filemtime(FTP_DIR."ALARM.CSV")){
			$log->logPrint("old file delete", REC_SOCK,__LINE__,__FILE__);

			touch(FTP_DIR.'ALARM.FLG');
			$log->logPrint("touch(".FTP_DIR.'ALARM.FLG'.")", REC_SOCK,__LINE__,__FILE__);

			$unlinkReturn = unlink(FTP_DIR."ALARM.CSV");
			$log->logPrint("unlink(".FTP_DIR."ALARM.CSV".")", REC_SOCK,__LINE__,__FILE__);
			$log->logPrint("unlink return:[$unlinkReturn]", REC_SOCK,__LINE__,__FILE__);

			touch(FTP_DIR.'ALARM.CSV');
			$log->logPrint("touch(".FTP_DIR.'ALARM.CSV'.")", REC_SOCK,__LINE__,__FILE__);

			$unlinkReturn = unlink(FTP_DIR.'ALARM.FLG');
			$log->logPrint("unlink(".FTP_DIR."ALARM.FLG".")", REC_SOCK,__LINE__,__FILE__);
			$log->logPrint("unlink return:[$unlinkReturn]", REC_SOCK,__LINE__,__FILE__);
		}
	}

	private function to_alarm_place($pnt_id){
		#TODO 発令地域コードに変換する
		$lines = file($this->getPath("TMP_ALARM_PLACE_PATH"));

		foreach($lines as $line){
			$tokens = explode(",", $line);
			$pnt_ids = explode(":", $tokens[2]);
			$pnt_ids[count($pnt_ids) - 1] = trim($pnt_ids[count($pnt_ids) - 1]);

			if(in_array($pnt_id, $pnt_ids)){
				return $tokens[0];
			}
		}

		return "";
	}

	private function getDate($warning){
		preg_match('/([0-9]{4})\/([0-9]{2})\/([0-9]{2})/', $warning[0][0], $m);
		return $m[1].$m[2].$m[3];
	}

	private function getTime($warning){
		preg_match('/([0-9]{2}:[0-9]{2})/', $warning[0][0], $m);
		return $m[1];
	}

	private function getMode($warning){
//		preg_match('/(注意報|警報)$/', mb_convert_encoding($warning[0][1], "EUC-JP", "UTF-8"), $m);
		preg_match('/(注意報|警報)$/', $warning[0][1], $m);

		if($m[1] == '注意報'){
			return 1;
		}else if($m[1] == '警報'){
			return 2;
		}else{
			return 0;
		}
	}

	private function getMessage($warning){
		var_dump($warning);
	}

	private function get($warning, $index1, $index2){
//		$pattern = mb_convert_encoding("/設定値　|実測値　/", "UTF-8", "EUC-JP");
		$pattern = "/設定値　|実測値　/";
		return preg_replace($pattern, "", $warning[$index1][$index2], 1);
	}
}