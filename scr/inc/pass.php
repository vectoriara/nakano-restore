<?
//ɸ������ե�����
if(!defined("CONST_INC"))		include("const.inc");
if(!defined("COMMON_INC"))	include(ROOT_INC."common.inc");


if(!$stdin=fopen("php://stdin", "r")){
	print "������ץȳ��ϤǤ��ޤ���";
}

print "�����ϲ������Ǥ�����\n>";
$s = trim(fgets($stdin, 4096));

if(strtolower($s)=="exit") exit();

if( $s != "okakatomentaiko" ){
	print "�ҥ�ȡ�������".date("Yǯm��d��")."�Ǥ�\n";
	exit();
}

$dbs= new CONN;


for(;;){

	print "\n�⡼�ɡ�\n>";
	$mode = trim(fgets($stdin, 4096));

	switch($mode){
		case "exit":
		case "EXIT":
			exit();
			break;

		case "h":
			print "���������桼���ɲ�\n";
			print "���������桼������\n";
			print "���������桼�����\n";
			print "�����������桼�����\n";
			print "���������桼���Ȳ�\n";
			print "���������桼������\n";
			break;

		case "6":
			$row=$dbs->Query("SELECT * FROM t_admmst;");
			while($row){
				print "\n";
				print "�桼��̾        ��".$row["adm_id"]."\n";
				print "�ѥ����      ��".$row["adm_pwd"]."\n";
				print "������٥�      ��".$row["adm_level"]."\n";
				print "�᡼�륢�ɥ쥹  ��".$row["adm_mail"]."\n";
				print "\n";
				$row = $dbs->Next();
			}

			break;

		case "5":
			print "�Ȳ񤹤�����桼����\n>";
			$id = trim(fgets($stdin, 4096));
			if(GetRecordCount("t_admmst","adm_id = '{$id}'")==0){
				print "¸�ߤ��ʤ������桼���Ǥ�\n";
				break;
			}
			$row=$dbs->Query("SELECT * FROM t_admmst WHERE adm_id = '{$id}';");
			print "\n";
			print "�桼��̾        ��".$row["adm_id"]."\n";
			print "�ѥ����      ��".$row["adm_pwd"]."\n";
			print "������٥�      ��".$row["adm_level"]."\n";
			print "�᡼�륢�ɥ쥹  ��".$row["adm_mail"]."\n";
			print "\n";
			break;
		case "4":
			print "���٤Ƥδ����桼���������ޤ�������Ǥ�����\n>";
			$s = trim(fgets($stdin, 4096));

			if( $s != "okakatomentaiko" ){
				print "³�ԤǤ��ޤ���\n";
				break;
			}
			if( $dbs->Execute("DELETE FROM t_admmst;") ){
				print "�������桼���������ޤ���\n";
			}else{
				print "�������桼���κ���˼��Ԥ��ޤ���\n";
			}
			break;

		case "3":
			print "�����������桼����\n>";
			$id = trim(fgets($stdin, 4096));
			if(GetRecordCount("t_admmst","adm_id = '{$id}'")>0){
				print "������ޤ�������Ǥ�����\n(Y or N) >";

				if(strtolower(trim(fgets($stdin, 4096)))=="no") break;

				if( $dbs->Execute("DELETE FROM t_admmst WHERE adm_id = '{$id}';") ){
					print "�����桼���������ޤ���\n";
				}else{
					print "�����桼���κ���˼��Ԥ��ޤ���\n";
				}
			}else{
				print "¸�ߤ��ʤ������桼���Ǥ�\n";
				break;
			}
			break;

		case "2":
		case "1":
			if($mode=="2"){
				print "������������桼����\n>";
				$id = trim(fgets($stdin, 4096));
				if(GetRecordCount("t_admmst","adm_id = '{$id}'")==0){
					print "¸�ߤ��ʤ������桼���Ǥ�\n";
					break;
				}
			}else{
				print "�ɲä�������桼����\n>";
				$id = trim(fgets($stdin, 4096));
				if($id==""){
					print "�ɲä�������桼�������Ϥ��Ƥ�������\n";
					break;
				}
			}

			print "�ѥ���ɡ�\n>";
			$pwd = trim(fgets($stdin, 4096));
			if($pwd==""){
				print "�ѥ���ɤ����Ϥ��Ƥ�������\n";
				break;
			}
			print "�ѥ���ɤΰŹ沽��Ԥ��ޤ�����\n(Y or N) >";
			if(strtolower(trim(fgets($stdin, 4096)))=="y"){
				$old_pwd = $pwd;
				$pwd = crypt($pwd);
			}else{
				$old_pwd = "";
			}

			print "������٥롩\n>";
			$level = trim(fgets($stdin, 4096));
			if($level==""){
				print "������٥�����Ϥ��Ƥ�������\n";
				break;
			}
			if(!is_numeric($level)){
				print "������٥�����Ϥ��Ƥ�������\n";
				break;
			}

			print "�᡼�륢�ɥ쥹��\n>";
			$mail = trim(fgets($stdin, 4096));
			if($mail!=""){
				if(mailcheck($mail)){
					print "�������᡼�륢�ɥ쥹�����Ϥ��Ƥ�������\n";
					break;
				}
			}

			print "\n\n�ʲ������ƤǤ�����Ǥ�����\n\n";
			print "�桼��̾        ��{$id}\n";
			if($old_pwd==""){
				print "�ѥ����      ��{$pwd}\n";
			}else{
				print "�ѥ����      ��{$old_pwd}\n";
				print "�Ź沽�ѥ���ɡ�{$pwd}\n";
			}
			print "������٥�      ��{$level}\n";
			print "�᡼�륢�ɥ쥹  ��{$mail}\n";

			print "\n\n(Y or N) >";
			if(strtolower(trim(fgets($stdin, 4096)))=="n") break;

			if($mode=="2"){
				$strSql = "UPDATE t_admmst SET adm_pwd='{$pwd}',adm_level={$level},adm_mail = '{$mail}' where adm_id='$id';";
				if( $dbs->Execute($strSql) ){
					print "�����桼���򹹿����ޤ���\n";
				}else{
					print "�����桼���ι����˼��Ԥ��ޤ���\n";
				}
			}else{
				$strSql = "INSERT INTO t_admmst VALUES('{$id}','{$pwd}',$level,'{$mail}');";
				if( $dbs->Execute($strSql) ){
					print "�����桼�����ɲä��ޤ���\n";
				}else{
					print "�����桼�����ɲä˼��Ԥ��ޤ���\n";
				}
			}
			break;
	}
}
?>

