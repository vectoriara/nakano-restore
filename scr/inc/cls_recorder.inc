<?
//============================================================//
//	ログ記録クラス																						//
//------------------------------------------------------------//
//	処理概要																									//
//	　ログファイルの作成をおこなう														//
//------------------------------------------------------------//
//	変更履歴																									//
//		2006/11/10	N.ARAI																			//
//		・新規作成																							//
//============================================================//
class Recorder
{
	var $varsion	= "1.0";
	var $update		= "2007/01/20";

	//変数宣言
	var $file;
	var $type;
	//コンストラクタ
	function __construct($name = "",$type = "",$pnt = ""){
		//パス設定
		$this->file = LOG_DIR.date("Ymd").($pnt=="" ? "" : "_{$pnt}").($name=="" ? "" : "_".$name).".log";
		$this->type = $type;
	}

	//ログ記録処理
	function logPrint($msg,$level,$l,$f,$user_flg=0,$lf=0,$enc=""){
		//user_flg => web経由時などセッション情報を出力したい場合に１を設定する（ 初期値：0 ）
		//lf       => 改行コードをそのまま出力したい場合に１を設定する         （ 初期値：0 ）
		//enc      => メッセージに対してエンコードを書ける場合に設定
		//※ 例１："変換後エンコード,変換前のエンコード１,変換前のエンコード２...."

		$log = date("Y-m-d(D) H:i:s");

		//セッション情報出力
		if($user_flg){
			session_cache_limiter("public");
			session_start();
			$log .= " [".sprintf("%-15s",$_SERVER['REMOTE_ADDR']).":".sprintf("%-10s",$_SESSION["id"])."]  ";
		}

		switch($level){
			case REC_NOTE:
				$log .= " Note :";
				break;
			case REC_SOCK:
				$log .= " Sock :";
				break;
			case REC_ERROR:
				$log .= " ERROR:";
				break;
			case REC_ALERT:
				$log .= " ALERT:";
				break;
			case REC_DEBUG:
				if(!DEBUG) return true;
				$log .= " DEBUG:";
				break;
			default:
				$log .= "      :";
		}
		$log .= sprintf("%-15s",$this->type);

		if(!$lf) $msg = str_replace("\n", "", $msg);

		$msg = str_replace(chr(0),"(null)", $msg);

		//文字エンコード変換
		if(is_array($enc)){
			$strTo		= trim($enc[0]);
			$strFrom	= array();
			$flg			= 0;
			for($i=1;$i<count($enc);$i++){
				if(trim($enc[$i])!=""){
					$strFrom[] = trim($enc[$i]);
					$flg = 1;
				}
			}
			if($strTo!=""&&$flg){
				$msg = mb_convert_encoding($msg,$strTo,$strFrom);
			}
		}

		$log .= $msg;
		if(DEBUG)	$log .= "  ({$f}:{$l})";

		$fp = fopen($this->file,"a");
		if($fp){
			fputs($fp,$log."\n");
			fclose($fp);
			@chmod($this->file, 0777);
		}
		return true;
	}
}
?>