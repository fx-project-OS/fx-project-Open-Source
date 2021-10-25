<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : personen_kompakt_aus_pre.inc                                 //
// Version     : 21.2                                                         //
// Begin       : 2020-10-02                                                   //
// Last Change : 2020-10-02                                                   //
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
 * PF 166: Client etc. - Person select mask (will be called as an alternative program function from other person types as well)
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 21.2
 */

// Always display select mask in edit mode
$maske382_sf=true;
unset($index);

// Select field: [Person_alias]
// ...Use person type as literal
$Person_alias_lit=meldung($_ptype);
// ...No default
$Person_alias_ds=0;
// ...Submit persontype parameters to select function
$Person_alias_arg=array('modus'=>2, 'ptpa'=>$ptpa);
$maskenfeldwerte['person_alias']=$_cid.'_'.$_pid;
if($_pid > 0)
	$Person_alias_arg['pid']=$maskenfeldwerte['person_alias'];

// Sort Communication: [ksort]
$ksort_nv=true;
if($GLOBALS['fxpglobals']['action'] == FXP_DISPLAY)
{
	// 2 radio buttons: [a]lphabetically + [c]hronologically
	$ksort_nv=false;
	if(!fxIsArray($_POST) || !isset($_POST['ksort']) || !strlen($_POST['ksort']))
		$_POST['ksort']='a';
	$maskenfeldwerte['ksort']='~'.fieldRadio('ksort', $_POST['ksort'], array('a'=>meldung(2069,true), 'c'=>meldung(2070,true)));
}
?>