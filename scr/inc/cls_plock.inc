<?
class PLOCK
{
	var $pid;
	var $path;

	//コンストラクタ [2007/10/08]
	function __construct(&$log = ""){
		if(is_object($log))	$this->log = $log;
	}

	//ロックスタート
	function LockStart($file_id){
		$this->pid	= getmypid();
		$this->path = ROOT_INC."{$file_id}.lock";

		if(@file_exists($this->path)){
			if(is_object($this->log))	$this->log->logPrint("LockFile Exists",REC_NOTE,__LINE__,__FILE__);

			$line			= @file($this->path);
			$old_pid	= trim($line[0]);
			if(is_object($this->log))	$this->log->logPrint("Forced ending old_pid => [{$old_pid}]",REC_NOTE,__LINE__,__FILE__);

			$cmd = PATH_SUDO." -u root kill -9 {$old_pid}";
			if(is_object($this->log))	$this->log->logPrint("Forced ending COMMAND => [{$cmd}]",REC_NOTE,__LINE__,__FILE__);
			system($cmd);
		}

		if($fp = @fopen($this->path,"w")){
			fputs($fp,$this->pid);
			fclose ($fp);
			@chmod($this->path,0777);
			if(is_object($this->log))	$this->log->logPrint("LockStart => [".$this->pid."]",REC_NOTE,__LINE__,__FILE__);
		}
	}

	//ロックエンド
	function LockEnd(){
		if(is_object($this->log))	$this->log->logPrint("LockEnd[".$this->pid."]",REC_NOTE,__LINE__,__FILE__);
		@unlink($this->path);
	}
}
?>