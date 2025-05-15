<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : ext_mailer.inc                                               //
// Version     : 24.1                                                         //
// Begin       : 2020-11-19                                                   //
// Last Change : 2024-03-14                                                   //
// Author      : FeRox Management Consulting GmbH & Co. KG                    //
//               Adolf-Langer-Weg 11a, D-94036 Passau (Germany)               //
//               https://www.ferox.de - info@ferox.de                         //
// License     : GNU-GPL v3 (https://opensource.org/licenses/GPL-3.0)         //
// -------------------------------------------------------------------------- //
// fx-project - An open source PHP Project Managament Software                //
// Copyright  © FeRox Management Consulting GmbH & Co. KG                     //
// -------------------------------------------------------------------------- //
// This program is free software: you can redistribute it and/or modify       //
// it under the terms of the GNU General Public License as published by       //
// the Free Software Foundation, either version 3 of the License, or          //
// (at your option) any later version.                                        //
//                                                                            //
// This program is distributed in the hope that it will be useful,            //
// but WITHOUT ANY WARRANTY; without even the implied warranty of             //
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the              //
// GNU General Public License for more details.                               //
//                                                                            //
// You should have received a copy of the GNU General Public License          //
// along with this program.  If not, see <https://www.gnu.org/licenses/>.     //
//                                                                            //
// See ../LICENSE.TXT file for more information.                              //
// -------------------------------------------------------------------------- //
// LICENSING ADDENDUM:                                                        //
// Programs in the SPP (Special Programs) subfolder are coded extensions of   //
// the open source software fx-project. These programs are offered for sale   //
// by the manufacturer FeRox Management Consulting GmbH & Co. KG and require  //
// a valid key for execution. It is forbidden to resell these programs        //
// and/or keys or to pass them on free of charge or use them without the      //
// express written permission of FeRox Management Consulting GmbH & Co. KG.   //
////////////////////////////////////////////////////////////////////////////////

/**
 * @file
 * Check if Mailer is available
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 24.1
 */

$GLOBALS['__loaded_'.basename(__FILE__)]=true;

// PHPMailer namespace
use PHPMailer\PHPMailer\PHPMailer;
use PHPMailer\PHPMailer\Exception;

// PHPMailer language directory
$_ext_mailer_languagepath='';
if(isset($GLOBALS['_ext_array']['PHPMAILER']))
{
	$_mailer_path=$GLOBALS['_ext_array']['PHPMAILER'];
	while(strlen((string)$_mailer_path))
	{
		$_mailer_lsp=strrpos($_mailer_path,'/');
		if($_mailer_lsp === false)
			break;
		$_mailer_path=substr((string)$_mailer_path,0,$_mailer_lsp);
		if(is_dir($_mailer_path.'/language'))
		{
			$_ext_mailer_languagepath=$_mailer_path.'/language/';
			break;
		}
	}
}
define('MAILER_LANGUAGEPATH', $_ext_mailer_languagepath);
//echo('MAILER_LANGUAGEPATH: '.MAILER_LANGUAGEPATH.'<hr />');

// Mailer available?
$GLOBALS['_ext_mailer']='';
// ...PHPMAILER
if(!class_exists('PHPMailer\PHPMailer\PHPMailer') && isset($GLOBALS['_ext_array']['PHPMAILER']))
{
	$_mailer_lsp=strrpos($GLOBALS['_ext_array']['PHPMAILER'],'/');
	$_mailer_path=substr((string)$GLOBALS['_ext_array']['PHPMAILER'],0,$_mailer_lsp+1);

	// Include these files
	$_mailer_ifiles=array('Exception.php', 'PMPMailer.php', 'SMTP.php');
	foreach($_mailer_ifiles as $_mailer_ifile)
	{
		$_mailer_inc=$_mailer_path.$_mailer_ifile;
		if(file_exists($_mailer_inc))
		{
//echo('require: '.$_mailer_inc.'<br />');
			require($_mailer_inc);
		}
	}

	// Default
	if(!class_exists('PHPMailer\PHPMailer\PHPMailer'))
	{
//echo('require: '.$GLOBALS['_ext_array']['PHPMAILER'].'<br />');
		require($GLOBALS['_ext_array']['PHPMAILER']);
	}
}
if(class_exists('PHPMailer\PHPMailer\PHPMailer'))
	$GLOBALS['_ext_mailer']='PHPMAILER';

//fxDebug(get_declared_classes(),'classes', 0);
?>