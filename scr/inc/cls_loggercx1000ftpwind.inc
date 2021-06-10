<?
	class LoggerCX1000FTPWIND extends LoggerCX1000FTP{
		//変数宣言
		var $varsion		= "1.0";
		var $update			= "2013.01.18";
		var $memo				= "江津市風局用";
		var $id					= "CX1000FTPWIND";
		var $type				= "2";														//ロガータイプ　1 => 気象ロガー	 2 => 雨量ロガー

		//帳票毎でこのロガーが出力できる項目を定義
		var $OutField = array(
													"000" => array(	"avg_ws",
																					"avg_wd",
																					"max_ws",
																					"max_wd",
																					#"temp",
																					#"humid1",
																					#"humid2",
																					#"press1",
																					#"press2",
																					#"rain_10min",
																					#"rain_hour",
																					#"rain_day",
																					#"move_rain_10min",
																					#"move_rain_hour",
																					#"move_rain_day",
																					#"rain_1min",
																					"weather",
																					#"rain_rui"
																				),
													"100" => array(	"avg_ws",
																					"avg_wd",
																					"max_ws",
																					"max_wd",
														#"temp",
														#"humid1",
														#"humid2",
														#"press1",
														#"rain_10min",
														#"rain_hour",
														#"rain_day",
														#"radiation"
																				),
													"200" => array(	"avg_ws",
																					"avg_wd",
																					"max_ws",
																					"max_wd",
																					#"temp",
																					#"humid1",
																					#"humid2",
																					#"press1",
																					#"max_rain_10min",
																					#"rain_hour",
																					#"rain_day",
																					#"radiation"
																				),
													"300" => array(	"avg_ws",
																					"avg_wd",
																					"max_ws",
																					"max_wd",
																					#"avg_temp",
																					#"max_temp",
																					#"min_temp",
																					#"avg_humid1",
																					#"max_humid1",
																					#"min_humid1",
																					#"humid2",
																					#"press1",
																					#"avg_press1",
																					#"max_press1",
																					#"min_press1",
																					#"max_rain_10min",
																					#"max_rain_hour",
																					#"rain_day",
																					#"radiation"
																				),
													"400" => array("avg_ws",
																			    "avg_wd",
																			    "max_ws",
																			    "max_wd",
//																			    "avg_temp",
//																			    "max_temp",
//																			    "min_temp",
//																			    "avg_humid1",
//																			    "max_humid1",
//																			    "min_humid1",
//																			    "humid2",
//																			    "avg_humid2",
//																			    "max_humid2",
//																			    "min_humid2",
//																			    "press1",
//																			    "avg_press1",
//																			    "max_press1",
//																			    "min_press1",
//																			    "max_rain_10min",
//																			    "max_rain_hour",
//																			    "max_rain_day",
//																			    "rain_month",
//																					"radiation"
																				)
													);

	}
?>