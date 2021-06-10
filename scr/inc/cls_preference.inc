<?
class PREFERENCE
{
	var $pid;
	var $path;
	var $wait_list;
	var $exec_list;
	var $wait_path;
	var $exec_path;

	//コンストラクタ [2007/10/08]
	function __construct(&$log = ""){
		if(is_object($log))	$this->log = $log;

		$this->wait_list	= ROOT_DIR."preference/wait/";
		$this->exec_list	= ROOT_DIR."preference/exec/";

		if(!is_dir($this->wait_list)){
			$cmd = PATH_SUDO." -u root mkdir -p ".$this->wait_list;
			if(is_object($this->log))	$this->log->logPrint("[{$cmd }]",REC_DEBUG,__LINE__,__FILE__);
			exec($cmd);
		}

		if(!is_dir($this->exec_list)){
			$cmd = PATH_SUDO." -u root mkdir -p ".$this->exec_list;
			if(is_object($this->log))	$this->log->logPrint("[{$cmd }]",REC_DEBUG,__LINE__,__FILE__);
			exec($cmd);
		}

		$this->Resetpermission();
	}

	function Resetpermission(){
		$cmd = PATH_SUDO." -u root chown -R admin:admin ".ROOT_DIR."preference";
		if(is_object($this->log))	$this->log->logPrint("[{$cmd }]",REC_DEBUG,__LINE__,__FILE__);
		exec($cmd);
		$cmd = PATH_SUDO." -u root chmod -R 0777 ".ROOT_DIR."preference";
		if(is_object($this->log))	$this->log->logPrint("[{$cmd }]",REC_DEBUG,__LINE__,__FILE__);
		exec($cmd);
	}

	function StartPreference($id,$preference=0,$timeout=30){
		$guid							= date("YmdHis_").substr(md5(uniqid(microtime())),0,16);
		$filename					= $id."-".$preference."-".$guid;
		$this->wait_path	= $this->wait_list.$filename;
		$this->exec_path	= $this->exec_list.$filename;

		$cmd = "echo '".time()."' > ".$this->wait_path;
		if(is_object($this->log))	$this->log->logPrint("id[{$id}] preference[{$preference}] timeout[{$timeout}][{$cmd }]",REC_DEBUG,__LINE__,__FILE__);
		exec($cmd);

		$this->Resetpermission();

		$flg = false;
		for($i=0;$i<$timeout*5;$i++){
			if(file_exists($this->exec_path)){
				if(is_object($this->log))	$this->log->logPrint("[{$filename}]Execute",REC_DEBUG,__LINE__,__FILE__);
				$flg = true;
				break;
			}
			if(is_object($this->log))	$this->log->logPrint("[{$filename}]Wait",REC_DEBUG,__LINE__,__FILE__);
			usleep(200000);
		}
		$cmd = PATH_SUDO." -u root rm -f ".$this->wait_path;
		exec($cmd);

		if($flg){
			if(is_object($this->log))	$this->log->logPrint("StartPreference[{$filename}]Execute OK",REC_DEBUG,__LINE__,__FILE__);
		}else{
			if(is_object($this->log))	$this->log->logPrint("StartPreference[{$filename}]Execute Cancel",REC_DEBUG,__LINE__,__FILE__);
		}
		return $flg;
	}

	function CheckPreferenceLevel($id,$preference=0){
		$tmp = array();
		$cmd = "ls ".$this->wait_list.$id."-*";
		#if(is_object($this->log))	$this->log->logPrint("[{$cmd }]",REC_DEBUG,__LINE__,__FILE__);
		exec($cmd,$tmp);
		#if(is_object($this->log))	$this->log->logPrint("[\n".print_r($tmp,TRUE)."]",REC_DEBUG,__LINE__,__FILE__,0,1);
		if($tmp[0]!=""){
			$filename = basename($tmp[0]);
			$tmp			= explode("-",$filename);
			$level		= $tmp[1];

			if(is_object($this->log))	$this->log->logPrint("優先度チェック[".$preference."]>=[".$level."]",REC_NOTE,__LINE__,__FILE__);
			if($preference>=$level){
				if(is_object($this->log))	$this->log->logPrint("優先度高有り[".$filename."]",REC_NOTE,__LINE__,__FILE__);
				return true;
			}
		}
		if(is_object($this->log))	$this->log->logPrint("優先度高なし",REC_NOTE,__LINE__,__FILE__);
		return false;
	}

	function EndPreference(){
		$cmd = PATH_SUDO." -u root rm -f ".$this->exec_path;
		if(is_object($this->log))	$this->log->logPrint("EndPreference[{$cmd }]",REC_DEBUG,__LINE__,__FILE__);
		exec($cmd);
	}

	function CheckPreference($id,$timeout=30){
		$tmp = array();
		$cmd = "ls ".$this->wait_list.$id."-*";
		#if(is_object($this->log))	$this->log->logPrint("[{$cmd }]",REC_DEBUG,__LINE__,__FILE__);
		exec($cmd,$tmp);
		#if(is_object($this->log))	$this->log->logPrint("[\n".print_r($tmp,TRUE)."]",REC_DEBUG,__LINE__,__FILE__,0,1);
		if($tmp[0]!=""){
			$cmd = PATH_SUDO." -u root mv ".$tmp[0]." ".$this->exec_list;
			if(is_object($this->log))	$this->log->logPrint("[{$cmd }]",REC_DEBUG,__LINE__,__FILE__);
			exec($cmd);

			for($i=0;$i<$timeout*5;$i++){
				if(!file_exists($this->exec_list.basename($tmp[0]))){
					if(is_object($this->log))	$this->log->logPrint("[".basename($tmp[0])."]Relece Process",REC_DEBUG,__LINE__,__FILE__);
					break;
				}
				if(is_object($this->log))	$this->log->logPrint("[".basename($tmp[0])."]Hold Process",REC_DEBUG,__LINE__,__FILE__);
				usleep(200000);
			}
		}
	}

}
?>