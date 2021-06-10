<?
//標準設定ファイル
if(!defined("CONST_INC"))		include("const.inc");
if(!defined("COMMON_INC"))	include(ROOT_INC."common.inc");


if(!$stdin=fopen("php://stdin", "r")){
	print "スクリプト開始できません";
}

print "今日は何の日ですか？\n>";
$s = trim(fgets($stdin, 4096));

if(strtolower($s)=="exit") exit();

if( $s != "okakatomentaiko" ){
	print "ヒント：今日は".date("Y年m月d日")."です\n";
	exit();
}

$dbs= new CONN;


for(;;){

	print "\nモード？\n>";
	$mode = trim(fgets($stdin, 4096));

	switch($mode){
		case "exit":
		case "EXIT":
			exit();
			break;

		case "h":
			print "１．管理ユーザ追加\n";
			print "２．管理ユーザ更新\n";
			print "３．管理ユーザ削除\n";
			print "４．全管理ユーザ削除\n";
			print "５．管理ユーザ照会\n";
			print "６．管理ユーザ一覧\n";
			break;

		case "6":
			$row=$dbs->Query("SELECT * FROM t_admmst;");
			while($row){
				print "\n";
				print "ユーザ名        ：".$row["adm_id"]."\n";
				print "パスワード      ：".$row["adm_pwd"]."\n";
				print "管理レベル      ：".$row["adm_level"]."\n";
				print "メールアドレス  ：".$row["adm_mail"]."\n";
				print "\n";
				$row = $dbs->Next();
			}

			break;

		case "5":
			print "照会する管理ユーザ？\n>";
			$id = trim(fgets($stdin, 4096));
			if(GetRecordCount("t_admmst","adm_id = '{$id}'")==0){
				print "存在しない管理ユーザです\n";
				break;
			}
			$row=$dbs->Query("SELECT * FROM t_admmst WHERE adm_id = '{$id}';");
			print "\n";
			print "ユーザ名        ：".$row["adm_id"]."\n";
			print "パスワード      ：".$row["adm_pwd"]."\n";
			print "管理レベル      ：".$row["adm_level"]."\n";
			print "メールアドレス  ：".$row["adm_mail"]."\n";
			print "\n";
			break;
		case "4":
			print "すべての管理ユーザを削除しますよろしいですか？\n>";
			$s = trim(fgets($stdin, 4096));

			if( $s != "okakatomentaiko" ){
				print "続行できません\n";
				break;
			}
			if( $dbs->Execute("DELETE FROM t_admmst;") ){
				print "全管理ユーザを削除しました\n";
			}else{
				print "全管理ユーザの削除に失敗しました\n";
			}
			break;

		case "3":
			print "削除する管理ユーザ？\n>";
			$id = trim(fgets($stdin, 4096));
			if(GetRecordCount("t_admmst","adm_id = '{$id}'")>0){
				print "削除しますよろしいですか？\n(Y or N) >";

				if(strtolower(trim(fgets($stdin, 4096)))=="no") break;

				if( $dbs->Execute("DELETE FROM t_admmst WHERE adm_id = '{$id}';") ){
					print "管理ユーザを削除しました\n";
				}else{
					print "管理ユーザの削除に失敗しました\n";
				}
			}else{
				print "存在しない管理ユーザです\n";
				break;
			}
			break;

		case "2":
		case "1":
			if($mode=="2"){
				print "更新する管理ユーザ？\n>";
				$id = trim(fgets($stdin, 4096));
				if(GetRecordCount("t_admmst","adm_id = '{$id}'")==0){
					print "存在しない管理ユーザです\n";
					break;
				}
			}else{
				print "追加する管理ユーザ？\n>";
				$id = trim(fgets($stdin, 4096));
				if($id==""){
					print "追加する管理ユーザを入力してください\n";
					break;
				}
			}

			print "パスワード？\n>";
			$pwd = trim(fgets($stdin, 4096));
			if($pwd==""){
				print "パスワードを入力してください\n";
				break;
			}
			print "パスワードの暗号化を行いますか？\n(Y or N) >";
			if(strtolower(trim(fgets($stdin, 4096)))=="y"){
				$old_pwd = $pwd;
				$pwd = crypt($pwd);
			}else{
				$old_pwd = "";
			}

			print "管理レベル？\n>";
			$level = trim(fgets($stdin, 4096));
			if($level==""){
				print "管理レベルを入力してください\n";
				break;
			}
			if(!is_numeric($level)){
				print "管理レベルを入力してください\n";
				break;
			}

			print "メールアドレス？\n>";
			$mail = trim(fgets($stdin, 4096));
			if($mail!=""){
				if(mailcheck($mail)){
					print "正しいメールアドレスを入力してください\n";
					break;
				}
			}

			print "\n\n以下の内容でよろしいですか？\n\n";
			print "ユーザ名        ：{$id}\n";
			if($old_pwd==""){
				print "パスワード      ：{$pwd}\n";
			}else{
				print "パスワード      ：{$old_pwd}\n";
				print "暗号化パスワード：{$pwd}\n";
			}
			print "管理レベル      ：{$level}\n";
			print "メールアドレス  ：{$mail}\n";

			print "\n\n(Y or N) >";
			if(strtolower(trim(fgets($stdin, 4096)))=="n") break;

			if($mode=="2"){
				$strSql = "UPDATE t_admmst SET adm_pwd='{$pwd}',adm_level={$level},adm_mail = '{$mail}' where adm_id='$id';";
				if( $dbs->Execute($strSql) ){
					print "管理ユーザを更新しました\n";
				}else{
					print "管理ユーザの更新に失敗しました\n";
				}
			}else{
				$strSql = "INSERT INTO t_admmst VALUES('{$id}','{$pwd}',$level,'{$mail}');";
				if( $dbs->Execute($strSql) ){
					print "管理ユーザを追加しました\n";
				}else{
					print "管理ユーザの追加に失敗しました\n";
				}
			}
			break;
	}
}
?>

