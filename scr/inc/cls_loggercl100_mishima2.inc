<?
	//========================================================//
	//	RS0001���������饹									   //
	//--------------------------------------------------------//
	//	��������											   //
	//		�����ϥ������ѽ������饹							//
	//--------------------------------------------------------//
	//	�ѹ�����											   //
	//		2015/11/01	N.ARAI								  //
	//		����������										   //
	//========================================================//
class LoggerCL100_MISHIMA2 extends LoggerCL100_MISHIMA
{
	//�ѿ����
	var $varsion	= "1.0";
	var $update		= "2020.10.01";
	var $memo		= "";
	var $id			= "CL100_MISHIMA2" ;

	var $is_edit_weather = false;	// ŷ���Խ���ǽ�ե饰
	var $is_edit_warning = false;	// ����ģ�����Խ���ǽ�ե饰

	//Ģɼ��Ǥ��Υ����������ϤǤ�����ܤ����
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

						//Ģɼ��Ǥ��Υ��������Խ��Ǥ�����ܤ����
						var $EditField = array(
												"100" => array(
													"snow_level"
													)
												);
}
?>
