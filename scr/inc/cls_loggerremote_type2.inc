<?
//============================================================//
//	外部取込ロガークラス																			//
//------------------------------------------------------------//
//	処理概要																									//
//	　高岡（氷見）用																					//
//------------------------------------------------------------//
//	変更履歴																									//
//		2013/10/01	N.ARAI																			//
//		・新規作成																							//
//============================================================//
class LoggerREMOTE_TYPE2 extends LoggerREMOTE_TYPE0
{
	//変数宣言
	var $varsion	= "1.0";
	var $update		= "2013.10.01";
	var $memo			= "";
	var $id				= "REMOTE_TYPE2";

	var $flg_edit_rainrui = false;//累計雨量編集

	//帳票毎でこのロガーが出力できる項目を定義
	var $OutField = array(
		"000" => array(	"snow_level",
			"move_snow_10min",
			"move_snow_hour"
		),
		"100" => array(	"snow_level",
			"move_snow_10min",
			"move_snow_hour"
		),
		"200" => array(	"snow_level",
			"move_snow_hour"
		),
		"300" => array(	"snow_level",
			"snow_day"
		),
		"400" => array( "snow_level",
			"snow_month"
		)
	);

	var $OutFieldIdx = array(
		"100" => array(
			"snow_level"			=> 13,
			"move_snow_10min"	=> 14,
			"move_snow_hour"	=> 15
		),
		"200" => array(
			"snow_level"			=> 13,
			"move_snow_hour"	=> 14
		),
		"300" => array(
			"snow_level"			=> 17,
			"snow_day"				=> 18
		),
		"400" => array(
			"snow_level"			=> 18,
			"snow_month"			=> 19
		)
	);

	var $OutSumFieldIdx = array(
		"200" => array(
			"snow_level"			=> array(50,55),
			"move_snow_hour"	=> array(50,55,70)
		),
		"300" => array(
			"snow_level"			=> array(50,55),
			"snow_day"				=> array(50,55,70)
		),
		"400" => array(
			"snow_level"			=> array(50,55),
			"snow_month"			=> array(50,55,70)
		)

	);

}
?>