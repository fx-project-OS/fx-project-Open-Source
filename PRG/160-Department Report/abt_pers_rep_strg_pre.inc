<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : abt_pers_rep_strg_pre.inc                                    //
// Version     : 24.1                                                         //
// Begin       : 2020-10-26                                                   //
// Last Change : 2020-10-26                                                   //
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
 * PF 160: Department Report
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 24.1
 */

$maske364_sf=true;

// Abteilung
$Abt_ID_ds=0;
$maskenfeldwerte['abt_id']=$_POST['Abt_ID'];

// Filter
$flt=filter_person('and', true, false, 'Personen_ID');

// Personen (nur int. + ext. MA)
$Personen_ID_ds=1410;
$Personen_ID_arg=array('modus'=>2, 'ptpa'=>array('persontypes'=>array(FXP_PT_EMPLOYEE,FXP_PT_CONTRACTOR), 'all_clients'=>false, 'noclient'=>true, 'filter'=>$flt));
if(!isset($_POST['Personen_ID']))
	$_POST['Personen_ID']=$GLOBALS['fxpglobals']['person'];
$maskenfeldwerte['personen_id']=$_POST['Personen_ID'];
?>