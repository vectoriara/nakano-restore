<?
class CONN
{
	//変数宣言
	var $Server;
	var $Port;
	var $Username;
	var $Password;
	var $Database;

	var $MyConn;
	var $MyRow;
	var $MyCurrentRow;
	var $Error;
	var $varsion = "1.0";

	//コンストラクタ
	function CONN(){
		$this->Server		=	DB_SERVER;		//サーバ名
		$this->Port			=	DB_PORT;			//ポート番号
		$this->Database	=	DB_NAME;			//データベース
		$this->Username	=	DB_USER;			//ユーザ名
		$this->Password	=	DB_PASS;			//パスワード

		$this->MyConn				=	0;			//接続ID
		$this->MyRow				=	0;			//現在の抽出行
		$this->MyCurrentRow	=	0;			//現在の対象行
		$this->Error				=	"";			//エラー文字列
	}

	//接続テスト
	function TestConnect(){
		//エラークリア
		$this->Error = "";
		$this->ErrNo = 0;

		$conn_string = "host=".$this->Server." port=".$this->Port." dbname=postgres user=".$this->Username." password=".$this->Password;
		//DB接続
		if( !$this->MyConn = @pg_connect($conn_string) ){ // 接続
			$this->Error = "データベースに接続できませんでした";
			return false;
		}
		return true;
	}

	//接続開始
	function Connect(){
		//エラークリア
		$this->Error = "";
		$this->ErrNo = 0;

		$conn_string = "host=".$this->Server." port=".$this->Port." dbname=".$this->Database." user=".$this->Username." password=".$this->Password;
		//DB接続
		if( !$this->MyConn = pg_connect($conn_string) ){ // 接続
			$this->Error = "データベースに接続できませんでした";
			return false;
		}
		return true;
	}

	//クエリ発行
	function Query( $sqlString ){
		//接続確認
		if( !$this->MyConn ){
			if( !$this->Connect( ) )	return 0;
		}

		// クエリ発行
		$this->MyCurrentRow=0;
		if( !$this->MyRow = @pg_exec($sqlString) ){ // クエリ発行
			$this->Error="query:".pg_errormessage()."::".$sqlString;
			return 0;
		}
		// 残り行数と比較
		if( pg_numrows($this->MyRow)<=$this->MyCurrentRow ){
			$this->MyCurrentRow=0;
			return 0;
		}
		// 結果取得
		if( !$row = @pg_fetch_array($this->MyRow,$this->MyCurrentRow) ){
			$this->Error="query:".pg_errormessage();
			return 0;
		}
		$this->MyCurrentRow++;
		return $row;
	}

	//コミット
	function Commit(){
		// 接続確認
		if( !$this->MyConn ){
			if( !$this->Connect( ) )	return 0;
		}

		// クエリ発行
		if( !$this->MyRow = @pg_exec("end;") ){ // クエリ発行
			$this->Error="execute:".pg_errormessage()."::".$sqlString;
			return 0;
		}
		return 1;
	}

	//トランザクション開始
	function BeginTrans(){
		// 接続確認
		if( !$this->MyConn ){
			if( !$this->Connect( ) )	return 0;
		}

		//クエリ発行
		if( !$this->MyRow = @pg_exec("begin;") ){ //クエリ発行
			$this->Error="execute:".pg_errormessage()."::".$sqlString;
			return 0;
		}
		return 1;
	}

	// Rollback
	function Rollback(){
		// 接続確認
		if( !$this->MyConn ){
			if( !$this->Connect( ) )	return 0;
		}
		//クエリ発行
		if( !$this->MyRow = @pg_exec("rollback;") ){ // クエリ発行
			$this->Error="execute:".pg_errormessage()."::".$sqlString;
			return 0;
		}
		return 1;
	}

	//クエリ発行（結果行なし）
	function Execute( $sqlString ){
		//接続確認
		if( !$this->MyConn ){
			if( !$this->Connect( ) )	return 0;
		}

		//クエリ発行
		if( !$this->MyRow = @pg_exec($sqlString) ){ // クエリ発行
			$this->Error="execute:".pg_errormessage()."::".$sqlString;
			return 0;
		}
		return 1;
	}

	//クエリ発行後の次行取り込み
	function Next(){
		//接続確認
		if( !$this->MyConn || !$this->MyRow ){
			return 0;
		}

		//残り行数と比較
		if( pg_numrows($this->MyRow)<=$this->MyCurrentRow ){
			$this->MyCurrentRow=0;
			return 0;
		}

		//次行取得
		if( !$row = @pg_fetch_array($this->MyRow,$this->MyCurrentRow) ){		// 結果取得
			$this->Error="next:".pg_errormessage();
			$this->MyCurrentRow=0;
			return 0;
		}
		$this->MyCurrentRow++;
		return $row;
	}

	//結果行の開放
	function Free(){
		// 接続確認
		if( !$this->MyConn || !$this->MyRow ){
			return 0;
		}
		//結果行メモリ開放
		pg_freeresult( $this->MyRow );
		$this->MyCurrentRow=0;
		return 0;
	}

	//一番初めに戻る
	function MoveFirst(){
		$this->MyCurrentRow=1;
		return 0;
	}

	//最終エラー取得
	function getError(){
		return $this->Error;
	}

	//全件取得
	function Query_All( $sqlString ){
		//接続確認
		if( !$this->MyConn ){
			if( !$this->Connect( ) )	return 0;
		}
		$hwnd = pg_query($this->MyConn,$sqlString);
		$row	= pg_fetch_all($hwnd);
		return $row;
	}

	//テーブル構成取得
	function GetMetaData( $strTable ){
		if( !$this->MyConn ){
			if( !$this->Connect( ) )	return 0;
		}
		return @pg_meta_data($this->MyConn,$strTable);
	}

	//シーケンス最終配布番号取得
	function GetSeqLastValue( $strSeq ){
		if( !$this->MyConn ){
			if( !$this->Connect( ) )	return 0;
		}
		if( !$scr = @pg_exec("SELECT last_value FROM $strSeq;")) return 0;
		if( !$row = @pg_fetch_array($scr,0) ) return 0;
		return $row["last_value"];
	}

}
?>
