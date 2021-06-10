<?
class DIRS
{

	function SearchDir($dir){
		$AryList = array();

		if($dh = @opendir($dir)){
			while(($file = @readdir($dh)) !== false){
				$path = $dir.$file;

				if($file=="."||$file=="..") continue;

				if(is_dir($path)){
					$AryList = array_merge($AryList,$this->SearchDir($path."/"));
				}else{
					$AryTmp = array();
					$AryTmp["path"] 			= $path;
					$AryTmp["file"] 			= $file;
					$AryTmp["dir"]				= str_replace(ROOT_DIR,"",$dir);
					$AryTmp["time"] 			= filemtime($path);
					$AryTmp["date"] 			= date("Y/m/d H:i",$AryTmp["time"]);
					$AryTmp["size"] 			= filesize($path);
					$AryTmp["perms"]			= $this->GetPerms($path);
					$AryTmp["perms_num"]	= $this->GetPermsNum($path);
					$AryTmp["owner"]			= $this->GetOwner($path);
					$AryTmp["group"]			= $this->GetGroup($path);
					$tmp_dir  = $AryTmp["dir"]=="" ? "/" : "";
					$AryList[ $tmp_dir.$AryTmp["dir"].$AryTmp["file"] ] = $AryTmp;
				}
			}
			closedir($dh);
		}
		return $AryList;
	}

	function GetPermsNum($path){
		return substr(sprintf('%o',fileperms($path)),-4);
	}

	function GetPerms($path){
		$perms = fileperms($path);

		if(($perms & 0xC000) == 0xC000){
			// ソケット
			$info = 's';
		}elseif(($perms & 0xA000) == 0xA000){
			// シンボリックリンク
			$info = 'l';
		}elseif(($perms & 0x8000) == 0x8000){
			// 通常のファイル
			$info = '-';
		}elseif(($perms & 0x6000) == 0x6000){
			// ブロック
			$info = 'b';
		}elseif(($perms & 0x4000) == 0x4000){
			// ディレクトリ
			$info = 'd';
		}elseif(($perms & 0x2000) == 0x2000){
			// キャラクタ
			$info = 'c';
		}elseif(($perms & 0x1000) == 0x1000){
			// FIFO パイプ
			$info = 'p';
		}else{
			// 不明
			$info = 'u';
		}

		// 所有者
		$info .= (($perms & 0x0100) ? 'r' : '-');
		$info .= (($perms & 0x0080) ? 'w' : '-');
		$info .= (($perms & 0x0040) ?
			(($perms & 0x0800) ? 's' : 'x' ) :
			(($perms & 0x0800) ? 'S' : '-'));
		// グループ
		$info .= (($perms & 0x0020) ? 'r' : '-');
		$info .= (($perms & 0x0010) ? 'w' : '-');
		$info .= (($perms & 0x0008) ?
			(($perms & 0x0400) ? 's' : 'x' ) :
			(($perms & 0x0400) ? 'S' : '-'));
		// 全体
		$info .= (($perms & 0x0004) ? 'r' : '-');
		$info .= (($perms & 0x0002) ? 'w' : '-');
		$info .= (($perms & 0x0001) ?
			(($perms & 0x0200) ? 't' : 'x' ) :
			(($perms & 0x0200) ? 'T' : '-'));
		return $info;
	}

	function GetOwner($path){
		$AryTmp = posix_getpwuid(fileowner($path));
		return $AryTmp["name"];
	}

	function GetGroup($path){
		$AryTmp =  posix_getgrgid (filegroup($path));
		return $AryTmp["name"];
	}

}
?>