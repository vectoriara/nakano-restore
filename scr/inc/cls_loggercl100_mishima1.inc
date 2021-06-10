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
class LoggerCL100_MISHIMA1 extends LoggerCL100_MISHIMA
{
	//変数宣言
	var $varsion	= "1.0";
	var $update		= "2020.10.01";
	var $memo		= "";
	var $id			= "CL100_MISHIMA1" ;

	var $is_edit_weather = false;	// 天気編集機能フラグ
	var $is_edit_warning = false;	// 気象庁警報編集機能フラグ


	//帳票毎でこのロガーが出力できる項目を定義
	var $OutField = array(
							"000" => array(
											"avg_wd",
											"avg_ws",
											"max_ws",
											"max_wd",
											"temp",
											"humid1",
											"rain_hour",
											"rain_day",
											"move_rain_10min",
											"move_rain_hour",
											"rain_rui",
											"avg__temp",
											"max__temp",
											"min__temp",
											"mainte"
											),
							"100" => array(
											"avg_ws",
											"avg_wd",
											"max_ws",
											"max_wd",
											"temp",
											"rain_10min",
											"move_rain_10min",
											"max__move_rain_10min",
											"max__move_rain_10min_time",
											"rain_hour",
											"move_rain_hour",
											"max__move_rain_hour",
											"max__move_rain_hour_time",
											"rain_day",
											"max__avg_ws",
											"max__avg_ws_time",
											"max__avg_wd",
											"max__max_ws_time",
											"max__temp",
											"max__temp_time",
											"min__temp",
											"min__temp_time",
											"humid1",
											"max__humid1",
											"max__humid1_time",
											"min__humid1",
											"min__humid1_time",
											"mainte"
											),
							"200" => array(
											"avg_ws",
											"avg_wd",
											"max_ws",
											"max_wd",
											"temp",
											"humid1",
											"max__humid1",
											"max__humid1_time",
											"min__humid1",
											"min__humid1_time",
											"max_rain_10min",
											"max__move_rain_10min",
											"rain_hour",
											"rain_day",
											"mainte"
											),
							"300" => array(
											"avg_ws",
											"avg_wd",
											"max_ws",
											"max_wd",
											"avg_temp",
											"max_temp",
											"min_temp",
											"avg_humid1",
											"max_humid1",
											"min_humid1",
											"max_rain_10min",
											"max_rain_hour",
											"rain_day"
											),
							"400" => array(
											"avg_ws",
											"avg_wd",
											"max_ws",
											"max_wd",
											"avg_temp",
											"max_temp",
											"min_temp",
											"avg_humid1",
											"max_humid1",
											"min_humid1",
											"max_rain_10min",
											"max_rain_hour",
											"max_rain_day",
											"rain_month"
											)
							);

						//帳票毎でこのロガーが編集できる項目を定義
						var $EditField = array(
												"100" => array(
													"avg_ws",
													"max__avg_ws",
													"avg_wd",
													"max__avg_wd",
													"max__avg_ws_time",
													"max_ws",
													"max__max_ws_time",
													"max_wd",
													"temp",
													"max__temp",
													"max__temp_time",
													"min__temp",
													"min__temp_time",
													"rain_10min",
													"max__move_rain_10min",
													"max__move_rain_10min_time",
													"rain_hour",
													"move_rain_hour",
													"max__move_rain_hour",
													"max__move_rain_hour_time",
													"rain_day",
													"humid1",
													"max__humid1",
													"max__humid1_time",
													"min__humid1_time",
													"min__humid1",
													"min__humid1_time"
													)
												);
}
?>

