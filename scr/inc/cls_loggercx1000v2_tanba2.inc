<?
//==========================================================//
//	CX1000ロガークラス										//
//----------------------------------------------------------//
//	処理概要												//
//	　CX1000用												//
//	　丹波市用として再調整バージョン						//
//----------------------------------------------------------//
//	変更履歴												//
//		2019/02/13	N.ARAI									//
//		・新規作成											//
//==========================================================//
class LoggerCX1000V2_TANBA2 extends LoggerCX1000V2
{
	//変数宣言
	var $varsion	= "1.0";
	var $update		= "2019.02.13";
	var $memo		= "";
	var $id			= "CX1000V2_TANBA2";
	var $SettingTemplate = "lgr_cx1000v2.tmpl";
	var $cx1000_input_lgr_snow = false;	// ロガー設定項目に積雪入力切替の有無（CX1000限定）

	//帳票毎でこのロガーが出力できる項目を定義
	var $OutField = array(
		"000" => array(
			"avg_ws",
			"avg_wd",
			"max_ws",
			"max_wd",
			"temp",
			"humid1",
			"humid2",
			"rain_10min",
			"rain_hour",
			"rain_day",
			"move_rain_10min",
			"move_rain_hour",
			"move_rain_day",
			"rain_1min",
			"weather",
			"rain_rui"
		),
		"100" => array(
			"avg_ws",
			"avg_wd",
			"max_ws",
			"max_wd",
			"temp",
			"humid1",
			"humid2",
			"rain_10min",
			"rain_hour",
			"rain_day",
			"rain_10min_max",
			"rain_10min_max_time",
			"move_rain_hour_max",
			"move_rain_hour_max_time"
		),
		"200" => array(
			"avg_ws",
			"avg_wd",
			"max_ws",
			"max_wd",
			"temp",
			"humid1",
			"humid2",
			"max_rain_10min",
			"rain_hour",
			"rain_day",
			"weather"
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
			"humid2",
			"max_rain_10min",
			"max_rain_hour",
			"rain_day",
			"weather1",
			"weather2",
			"weather3"
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
			"humid2",
			"avg_humid2",
			"max_humid2",
			"min_humid2",
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
			"avg_wd",
			"max_ws",
			"max_wd",
			"temp",
			"humid1",
			"humid2",
			"rain_10min",
			"rain_hour",
			"rain_day",
			"rain_10min_max",
			"rain_10min_max_time",
			"move_rain_hour_max",
			"move_rain_hour_max_time"
		),
		"200" => array(
			"avg_ws",
			"avg_wd",
			"max_ws",
			"max_wd",
			"temp",
			"humid1",
			"humid2",
			"max_rain_10min",
			"rain_hour",
			"rain_day",
			"weather"
		)
	);
}
?>