<?php
if(!defined("CONST_INC"))	include("const.inc");
if(!defined("COMMON_INC"))	include(ROOT_INC."common.inc");

if($_SERVER["argv"][0]!=basename(__FILE__)){
	// print "exec NG\n";	
	exit();
}

if(!$stdin=fopen("php://stdin", "r")){
	print "スクリプト開始できません";
	exit();
}

print "パスワード？\n>";
$s = trim(fgets($stdin, 4096));

if(strtolower($s)=="exit") exit();

if( crypt($s,"$1") != "$1WzPerce233I" ){
	print "ヒント：会社名は？\n";
	exit();
}

$aryLinux = array(
	array("root","aneoswebsys"),
	array("admin","aneoswebsys"),
);

$aryUser = array(
	array("root",	"aneoswebsys",1,5,""),
	array("sysadm",	"aneoswebsys",1,4,""),
	array("admin",	"aneoswebsys",1,3,""),
	array("op",		"aneoswebsys",1,2,""),
	array("user",	"aneoswebsys",1,1,"")
);

// $aryWebmin = array(
// 	array("root","aneoswebsys"),
// 	array("admin","aneoswebsys"),
// );

$aryChmod = array();
$aryChmod[] = "chmod 0777 ".ROOT_INC;
$aryChmod[] = "chmod 0777 ".ROOT_INC."db.conf";
$aryChmod[] = "chmod 0777 ".ROOT_INC."ntp.conf";
$aryChmod[] = "chmod 0644 ".ROOT_INC."setting.conf";
$aryChmod[] = "chmod 0777 ".ROOT_INC."pwd_warning.flg";
$aryChmod[] = "chmod 0777 ".ROOT_INC."pwd_weather.flg";
$aryChmod[] = "chmod 0777 ".LOG_DIR;
$aryChmod[] = "chmod 0777 ".WEATHER_DIR;
$aryChmod[] = "chmod 0777 ".DATA_DIR;
$aryChmod[] = "chmod 0777 ".XML_DIR;
$aryChmod[] = "chmod 0777 ".FTP_DIR;

print "初期管理ユーザの登録を行いますか？ (y/N)\n>";
$s = trim(fgets($stdin, 4096));
if(strtolower($s)=="y"){
	$dbs= new CONN;
	print "DELETE FROM t_admmst;\n";
	if(!$dbs->Execute("DELETE FROM t_admmst;")){
		print $dbs->getError()."\n";
		exit();
	}
	foreach($aryUser as $val){
		$id		= $val[0];
		$pwd	= $val[1];
		$crypt	= $val[2];
		$level	= $val[3];
		$mail	= $val[4];

		if($crypt) $pwd = crypt($pwd);
		$strSql = "INSERT INTO t_admmst VALUES('{$id}','{$pwd}',$level,'{$mail}');";
		print "INSERT INTO t_admmst VALUES('{$id}','{$pwd}',$level,'{$mail}');\n";
		if( !$dbs->Execute($strSql) ){
			print "管理ユーザの追加に失敗しました\n";
			print $dbs->getError()."\n";
			exit();
		}
	}
	print "管理ユーザの登録が完了しました\n";
}else{
	print "管理ユーザの登録はされませんでした\n\n";
}

print "フォルダ属性を変更しますか？ (y/N)\n>";
$s = trim(fgets($stdin, 4096));
if(strtolower($s)=="y"){
	foreach($aryChmod as $cmd){
		print $cmd."\n";
		exec($cmd);
	}
	print "フォルダ属性を変更しました\n\n";
}else{
	print "フォルダ属性は変更されませんでした\n\n";
}

print "#########################\n";
print "##     !!CAUTION!!     ##\n";
print "#########################\n";
print "Linuxユーザのパスワードを変更しますか？ (y/N)\n>";
$s = trim(fgets($stdin, 4096));
if(strtolower($s)=="y"){
	foreach($aryLinux as $val){
		$cmd = "echo \"".$val[0].":".$val[1]."\" | chpasswd";
		print $cmd."\n";
		exec($cmd);
		$cmd = "chage -l ".$val[0];
		print $cmd."\n";
		$ret = array();
		system($cmd);
		print "\n\n";
	}
	print "Linuxユーザのパスワードを変更しました\n";
}else{
	print "Linuxユーザのパスワードは変更されませんでした\n\n";
}

/* 
	途中までwebminパスワードを変更しようとしたが、セキュリティ上
	以降のバージョンでも同じ手法で通用するか不明なので現行バージョンでのコマンドラインの変更方法のみ以下に表示しておく。
	ただし、最も確実なのはブラウザで通常通りのパスワードを変更すること。

	必要な情報
	１．webminのインストール先（config格納先）： /etc/webmin
	２．パスワード変更スクリプトの場所 changepass.pl
		find / -name changepass.pl などで確認する
		結果例）
			[root@iws25-fukuchiyama inc]# find / -name changepass.pl
			/usr/local/src/webmin-1.962/changepass.pl

	３．変更コマンド書式
	　　usage: changepass.pl <config-dir> <login> <password>

	　　例）
			/usr/local/src/webmin-1.962/changepass.pl /etc/webmin admin aneoswebsys
		結果例）
			[root@iws25-fukuchiyama inc]# /usr/local/src/webmin-1.962/changepass.pl /etc/webmin admin aneoswebsys
			Updated password of Webmin user admin
	以上


print "#########################\n";
print "##     !!CAUTION!!     ##\n";
print "#########################\n";
print "Webminユーザのパスワードを変更しますか？ (y/N)\n>";
$s = trim(fgets($stdin, 4096));
if(strtolower($s)=="y"){

	$webmin_config_dir = "/etc/webmin";
	while(1){
		if(is_dir($webmin_config_dir)){
			print "Webmin Config Directory OK? ({$webmin_config_dir}) (y/[input path]])\n>";
			$s = trim(fgets($stdin, 4096));
			if(strtolower($s)=="y"){
				print "cat ".$webmin_config_dir."version\n";
				print "\n\nWebmin Version is [".system("cat ".$webmin_config_dir."/version")."]\n\n";
				print "Webmin Version Show OK? (y/n)\n>";
				$s = trim(fgets($stdin, 4096));
				if(strtolower($s)=="y"){
					break;
				}
			}else{
				$webmin_config_dir = $s;
			}
		}else{
			print "Input Wemin Config Directory\n>";
			$webmin_config_dir = trim(fgets($stdin, 4096));
		}
	}
	print "start Webmin change\n";
}else{
	print "Webminユーザのパスワードは変更されませんでした\n\n";
}
*/

?>