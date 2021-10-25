<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : personenauswahl_pre.inc                                      //
// Version     : 21.2                                                         //
// Begin       : 2020-10-20                                                   //
// Last Change : 2020-10-20                                                   //
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
 * Global: Called in various program functions that need a person selection
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 21.2
 */

$maske184_sf=true;

// Bei Transaktion 81: ZE (Tag) + 34: ZE (Summe) + 150: Reisekostenerfassung handeln wir die Auswahlmaske anders
if(($GLOBALS['fxpglobals']['tr'] != 81) && ($GLOBALS['fxpglobals']['tr'] != 34) && ($GLOBALS['fxpglobals']['tr'] != 150))
{
	$all_clients=false;
	if($_ptype == FXP_PT_CLIENT)
		$all_clients=true;

	$Personen_ID_arg=array('modus'=>2, 'ptpa'=>array('persontypes'=>array($_ptype), 'all_clients'=>$all_clients));
}
else
{
	if(!isset($Personen_ID_arg))
		$Personen_ID_arg['modus']=0;

	$Personen_ID=$GLOBALS['fxpglobals']['person'];
	if(fxIsArray($_GET) && isset($_GET['tr']) && fxIsArray($GLOBALS['fxptarray']) && fxIsArray($GLOBALS['fxptarray']['backup']))
		$Personen_ID=(int)$GLOBALS['fxptarray']['backup']['personen_id'];
	if(fxIsArray($_POST) && isset($_POST['Personen_ID']))
		$Personen_ID=(int)$_POST['Personen_ID'];
}

$Personen_ID_ds=0;
if($GLOBALS['with_client_id'])
	$maskenfeldwerte['personen_id']=$GLOBALS['fxpglobals']['client'].'_'.$Personen_ID;
else
	$maskenfeldwerte['personen_id']=$Personen_ID;
?>