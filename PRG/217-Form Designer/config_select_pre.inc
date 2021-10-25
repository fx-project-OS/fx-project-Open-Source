<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : config_select_pre.inc                                        //
// Version     : 21.2                                                         //
// Begin       : 2020-10-15                                                   //
// Last Change : 2020-10-15                                                   //
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
 * PF 217: Form Designer - Select Configuration File
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 21.2
 */

setForm(false);

// Mandantenordner (mordner)
if(!$GLOBALS['fxpglobals']['person'])
{
	$moa = array(0=>'(Default)');

	$mln = strlen((int)db_value("SELECT MAX(mandanten_id) FROM personen WHERE personentyp=274 AND mandanten_id>0"));
	$mar = db_values("SELECT mandanten_id FROM personen WHERE personentyp=274 AND mandanten_id>0 ORDER BY mandanten_id", '*');
	if(fxIsArray($mar))
	{
		foreach($mar as $m)
			$moa[(int)$m['mandanten_id']] = substr('000000000'.$m['mandanten_id'], -$mln);
	}
}
else if($GLOBALS['fxpglobals']['client'] == 1)
	$moa = array(0=>'(Default)', 1=>'1');
else
	$moa = array($GLOBALS['fxpglobals']['client']=>$GLOBALS['fxpglobals']['client']);

$mid = $GLOBALS['fxpglobals']['client'];
if(isset($_POST) && isset($_POST['mordner']))
	$mid = (int)$_POST['mordner'];
$mordner = $mid;

$maskenfeldwerte['mordner'] = '~'.fieldRadio('mordner', $mid, $moa, 'h', 10);

// Konfigurationstyp (ktyp)
$ktyp_ds = 0;
$ktyp = 2022;
if(isset($_POST) && isset($_POST['ktyp']))
	$ktyp = (int)$_POST['ktyp'];
$maskenfeldwerte['ktyp'] = $ktyp;

// Konfigurationsdatei (kdatei)
$kdatei_ds = 0;
$kdatei = '';
if(isset($_POST) && isset($_POST['kdatei']))
	$kdatei = $_POST['kdatei'];
$maskenfeldwerte['kdatei'] = $kdatei;

// Include select file
$req_fn=fxf_fn_reqFilename('config_select');
if(strlen($req_fn))
	require($req_fn);
?>