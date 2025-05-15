<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : personenauswahl_ini.inc                                      //
// Version     : 24.1                                                         //
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
 * @version 24.1
 */

$GLOBALS['with_client_id']=false;

// Bei Transaktion 81: ZE (Tag) und 34: ZE (Summe) handeln wir die Auswahlmaske etc. anders (in stunden_tag_liste_ini.inc)
// Bei Transaktion 150: Reisekostenerfassung handeln wir die Auswahlmaske etc. anders (in rk_tag_liste_ini.inc)
if(($GLOBALS['fxpglobals']['tr'] != 81) && ($GLOBALS['fxpglobals']['tr'] != 34) && ($GLOBALS['fxpglobals']['tr'] != 150))
{
	if(isset($_POST) && !isset($_POST['Personen_ID']) && isset($ppid))
	{
		$_POST['Personen_ID']=$ppid;
		$Personen_ID=$ppid;
	}
	if(isset($_POST) && $_POST['Personen_ID'])
	{
		protectVar($_POST['Personen_ID'], 'ppid');
		$mpa=explode('_',$_POST['Personen_ID']);
		if(sizeof($mpa) > 1)
		{
			$Personen_ID=(int)$mpa[1];
			$_POST['Personen_ID']=$Personen_ID;
			$GLOBALS['with_client_id']=true;
		}
		else
		{
			$Personen_ID=(int)$_POST['Personen_ID'];
			$_POST['Personen_ID']=$Personen_ID;
		}
	}
	if(isset($_ptype) && fxIsArray($GLOBALS['fxpglobals']['persdat']) && isset($GLOBALS['fxpglobals']['persdat']['personentyp']) && $GLOBALS['fxpglobals']['persdat']['personentyp'] && !$psuche)
	{
		if(($_ptype != $GLOBALS['fxpglobals']['persdat']['personentyp']) && !isset($Pname))
			unset($Personen_ID);
	}
	if(isset($_ptype) && isset($Personen_ID) && ((int)$Personen_ID != 0))
	{
		if($_ptype != db_value("SELECT personentyp FROM personen WHERE personen_id=".$Personen_ID." AND mandanten_id=".$GLOBALS['fxpglobals']['client']))
			unset($Personen_ID);
	}
	$GLOBALS['fxpglobals']['persdat']['personentyp']=$_ptype;

	if(!isset($PTyp))
		$PTyp=$GLOBALS['fxpglobals']['persdat']['personentyp'];

	// Richtige Person gewählt?
	if($db_update && $Palt && $_POST['Personen_ID'] && !$_POST['Person_waehlen'])
	{
		if($Palt != $_POST['Personen_ID'])
		{
			$_POST['Personen_ID']=$Palt;
			$Personen_ID=$Palt;
			error_msg(99, -1, 'Personen_ID');	// 99: Die Daten passen nicht zur <B>neu</B> gewählten Person. Die Person wurde angepasst!
			$err=true;
		}
	}

	protectVar($Personen_ID, 'Palt');

	if(($Palt != $Personen_ID) && ($GLOBALS['fxpglobals']['action'] != FXP_CREATE))
		unset($db_update);
}
?>