<?
	//========================================================//
	//	RS0001ロガークラス									   //
	//--------------------------------------------------------//
	//	処理概要											   //
	//		省電力ロガー用処理クラス							//
	//--------------------------------------------------------//
	//	変更履歴											   //
	//		2015/11/01	N.ARAI								  //
	//		・新規作成										   //
	//========================================================//
class LoggerCL100_MISHIMA2 extends LoggerCL100_MISHIMA
{
	//変数宣言
	var $varsion	= "1.0";
	var $update		= "2020.10.01";
	var $memo		= "";
	var $id			= "CL100_MISHIMA2" ;

	var $is_edit_weather = false;	// 天気編集機能フラグ
	var $is_edit_warning = false;	// 気象庁警報編集機能フラグ

	//帳票毎でこのロガーが出力できる項目を定義
	var $OutField = array(
							"000" => array(
											"snow_level",
											"mainte"
											),
							"100" => array(
											"snow_level",
											"mainte"
											),
							"200" => array(
											"snow_level",
											"mainte"
											),
							"300" => array(
											"snow_level"
											),
							"400" => array(
											"snow_level"
											)
							);

						//帳票毎でこのロガーが編集できる項目を定義
						var $EditField = array(
												"100" => array(
													"snow_level"
													)
												);
}
?>

