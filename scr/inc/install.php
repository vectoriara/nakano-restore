<?
if(!defined("CONST_INC"))		include("const.inc");
if(!defined("COMMON_INC"))	include(ROOT_INC."common.inc");


if(!$stdin=fopen("php://stdin", "r")){
	print "������ץȳ��ϤǤ��ޤ���";
	exit();
}

print "�����ϲ������Ǥ�����\n>";
$s = trim(fgets($stdin, 4096));

if(strtolower($s)=="exit") exit();

if( $s != "okakatomentaiko" ){
	print "�ҥ�ȡ�������".date("Yǯm��d��")."�Ǥ�\n";
	exit();
}

$default_user = array(
	array("root",		"neiwebsys",1,5,""),
	array("sysadm",	"neiwebsys",1,4,""),
	array("admin",	"neiwebsys",1,3,""),
	array("op",			"neiwebsys",1,2,""),
	array("user",		"neiwebsys",1,1,"")
);

print "��������桼������Ͽ��Ԥ��ޤ����� (y/n)\n>";
$s = trim(fgets($stdin, 4096));
if(strtolower($s)=="y"){
	$dbs= new CONN;
	print "DELETE FROM t_admmst;\n";
	if(!$dbs->Execute("DELETE FROM t_admmst;")){
		print $dbs->getError()."\n";
		exit();
	}
	foreach($default_user as $val){
		$id			= $val[0];
		$pwd		= $val[1];
		$crypt	= $val[2];
		$level	= $val[3];
		$mail		= $val[4];

		if($crypt) $pwd = crypt($pwd);
		$strSql = "INSERT INTO t_admmst VALUES('{$id}','{$pwd}',$level,'{$mail}');";
		print "INSERT INTO t_admmst VALUES('{$id}','{$pwd}',$level,'{$mail}');\n";
		if( !$dbs->Execute($strSql) ){
			print "�����桼�����ɲä˼��Ԥ��ޤ���\n";
			print $dbs->getError()."\n";
			exit();
		}
	}
	print "�����桼������Ͽ����λ���ޤ���\n";
}

print "�ե����°�����ѹ����ޤ����� (y/n)\n>";
$s = trim(fgets($stdin, 4096));
if(strtolower($s)=="y"){
	$tmp = array();
	$tmp[] = "chmod 0777 ".ROOT_INC;
	$tmp[] = "chmod 0777 ".ROOT_INC."db.conf";
	$tmp[] = "chmod 0777 ".ROOT_INC."ntp.conf";
	$tmp[] = "chmod 0644 ".ROOT_INC."setting.conf";
	$tmp[] = "chmod 0777 ".ROOT_INC."pwd_warning.flg";
	$tmp[] = "chmod 0777 ".ROOT_INC."pwd_weather.flg";
	$tmp[] = "chmod 0777 ".LOG_DIR;
	$tmp[] = "chmod 0777 ".WEATHER_DIR;
	$tmp[] = "chmod 0777 ".DATA_DIR;
	$tmp[] = "chmod 0777 ".XML_DIR;
	$tmp[] = "chmod 0777 ".FTP_DIR;

	foreach($tmp as $cmd){
		print $cmd."\n";
		exec($cmd);
	}
	print ROOT_DIR."\n";
	print "�ե����°�����ѹ����ޤ���\n\n";
}

print "ñ�ɤǤ����� (y/n)\n>";
$s = trim(fgets($stdin, 4096));
if(strtolower($s)=="y"){
	print "index.html���֤������ޤ����� (y/n)\n>";
	$s = trim(fgets($stdin, 4096));
	if(strtolower($s)=="y"){
		print "cp -f ".ROOT_DIR."index.html_d ".ROOT_DIR."index.html";
		print "\n";
		exec("cp -f ".ROOT_DIR."index.html_d ".ROOT_DIR."index.html");
		print "chmod 0644 ".ROOT_DIR."index.html";
		print "\n";
		exec("chmod 0644 ".ROOT_DIR."index.html");
		print "index.html���֤������ޤ���\n";
	}
}

print "���ե�������ѹ����ޤ����� (y/n)\n>";
$s = trim(fgets($stdin, 4096));
if(strtolower($s)=="y"){
	print "NEI or OK or ̵�� �Τɤ�Ǥ����� ( n / o / b )\n>";
	$s = trim(fgets($stdin, 4096));
	switch(strtolower($s)){
		case "n":
			$file = "nei_logo";
			break;
		case "o":
			$file = "ok_logo";
			break;
		case "b":
			$file = "nei_logo_blank.gif";
			break;
		default:
			print "���򥨥顼\n";
			exit();
	}

	if(strtolower($s)!="b"){
		print "ǯ�����ꤷ�Ƥ������� \n>";
		$s = trim(fgets($stdin, 4096));
		if(is_numeric($s)){
			$s = $s + 0;
			$file .= $s.".gif";
		}else{
			print "ǯ�椬�����������Ǥ��ޤ���\n";
			exit();
		}
	}

	$file		= ROOT_DIR."image/common/".$file;
	$file2	= ROOT_DIR."image/common/nei_logo.gif";
	if(@file_exists($file)){
		print "[{$file}]����Ѥ��ޤ�����Ǥ����� (y/n)\n>";
		$s = trim(fgets($stdin, 4096));
		if(strtolower($s)=="y"){
			print "cp -f {$file} {$file2}";
			print "\n";
			exec("cp -f {$file} {$file2}");
			print "chmod 0644 {$file2}";
			print "\n";
			exec("chmod 0644 {$file2}");
			print "[{$file}]��[{$file2}]���֤������ޤ���\n";
		}
	}else{
		print "���ե����뤬¸�ߤ��ޤ���\n";
		exit();
	}
}
print "��λ���ޤ���\n";
?>