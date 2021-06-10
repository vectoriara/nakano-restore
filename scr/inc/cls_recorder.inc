<?
//============================================================//
//	������Ͽ���饹																						//
//------------------------------------------------------------//
//	��������																									//
//	�������ե�����κ����򤪤��ʤ�														//
//------------------------------------------------------------//
//	�ѹ�����																									//
//		2006/11/10	N.ARAI																			//
//		����������																							//
//============================================================//
class Recorder
{
	var $varsion	= "1.0";
	var $update		= "2007/01/20";

	//�ѿ����
	var $file;
	var $type;
	//���󥹥ȥ饯��
	function __construct($name = "",$type = "",$pnt = ""){
		//�ѥ�����
		$this->file = LOG_DIR.date("Ymd").($pnt=="" ? "" : "_{$pnt}").($name=="" ? "" : "_".$name).".log";
		$this->type = $type;
	}

	//������Ͽ����
	function logPrint($msg,$level,$l,$f,$user_flg=0,$lf=0,$enc=""){
		//user_flg => web��ͳ���ʤɥ��å����������Ϥ��������ˣ������ꤹ��� ����͡�0 ��
		//lf       => ���ԥ����ɤ򤽤Τޤ޽��Ϥ��������ˣ������ꤹ��         �� ����͡�0 ��
		//enc      => ��å��������Ф��ƥ��󥳡��ɤ�񤱤��������
		//�� �㣱��"�Ѵ��奨�󥳡���,�Ѵ����Υ��󥳡��ɣ�,�Ѵ����Υ��󥳡��ɣ�...."

		$log = date("Y-m-d(D) H:i:s");

		//���å����������
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

		//ʸ�����󥳡����Ѵ�
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