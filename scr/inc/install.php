<?
if(!defined("CONST_INC"))		include("const.inc");
if(!defined("COMMON_INC"))	include(ROOT_INC."common.inc");


if(!$stdin=fopen("php://stdin", "r")){
	print "スクリプト開始できません";
	exit();
}

print "今日は何の日ですか？\n>";
$s = trim(fgets($stdin, 4096));

if(strtolower($s)=="exit") exit();

if( $s != "okakatomentaiko" ){
	print "ヒント：今日は".date("Y年m月d日")."です\n";
	exit();
}

$default_user = array(
	array("root",		"neiwebsys",1,5,""),
	array("sysadm",	"neiwebsys",1,4,""),
	array("admin",	"neiwebsys",1,3,""),
	array("op",			"neiwebsys",1,2,""),
	array("user",		"neiwebsys",1,1,"")
);

print "初期管理ユーザの登録を行いますか？ (y/n)\n>";
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
			print "管理ユーザの追加に失敗しました\n";
			print $dbs->getError()."\n";
			exit();
		}
	}
	print "管理ユーザの登録が完了しました\n";
}

print "フォルダ属性を変更しますか？ (y/n)\n>";
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
	print "フォルダ属性を変更しました\n\n";
}

print "単局ですか？ (y/n)\n>";
$s = trim(fgets($stdin, 4096));
if(strtolower($s)=="y"){
	print "index.htmlを置き換えますか？ (y/n)\n>";
	$s = trim(fgets($stdin, 4096));
	if(strtolower($s)=="y"){
		print "cp -f ".ROOT_DIR."index.html_d ".ROOT_DIR."index.html";
		print "\n";
		exec("cp -f ".ROOT_DIR."index.html_d ".ROOT_DIR."index.html");
		print "chmod 0644 ".ROOT_DIR."index.html";
		print "\n";
		exec("chmod 0644 ".ROOT_DIR."index.html");
		print "index.htmlを置き換えました\n";
	}
}

print "ロゴファイルを変更しますか？ (y/n)\n>";
$s = trim(fgets($stdin, 4096));
if(strtolower($s)=="y"){
	print "NEI or OK or 無し のどれですか？ ( n / o / b )\n>";
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
			print "選択エラー\n";
			exit();
	}

	if(strtolower($s)!="b"){
		print "年号を指定してください \n>";
		$s = trim(fgets($stdin, 4096));
		if(is_numeric($s)){
			$s = $s + 0;
			$file .= $s.".gif";
		}else{
			print "年号が正しく取得できません\n";
			exit();
		}
	}

	$file		= ROOT_DIR."image/common/".$file;
	$file2	= ROOT_DIR."image/common/nei_logo.gif";
	if(@file_exists($file)){
		print "[{$file}]を使用しますよろしですか？ (y/n)\n>";
		$s = trim(fgets($stdin, 4096));
		if(strtolower($s)=="y"){
			print "cp -f {$file} {$file2}";
			print "\n";
			exec("cp -f {$file} {$file2}");
			print "chmod 0644 {$file2}";
			print "\n";
			exec("chmod 0644 {$file2}");
			print "[{$file}]を[{$file2}]に置き換えました\n";
		}
	}else{
		print "ロゴファイルが存在しません\n";
		exit();
	}
}
print "終了しました\n";
?>