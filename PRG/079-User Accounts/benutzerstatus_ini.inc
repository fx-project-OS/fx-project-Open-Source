<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : benutzerstatus_ini.inc                                       //
// Version     : 24.1                                                         //
// Begin       : 2020-10-26                                                   //
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
 * PF 79: User Accounts
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 24.1
 */

// Execute database calls?
if(fxIsArray($_POST) && isset($_POST['Button_Multi']))
	$db_update=true;
else if(isset($db_update))
	unset($db_update);

// Set modus
$modarr=array(
	'Button_Anzeig'=>array(0, 358),
	'Button_Passwort_generieren'=>array(1, 1346),
	'Button_Sperre_aufheben'=>array(2, 1347),
	'Button_Benutzer_aktivieren'=>array(3, 1348),
	'Button_Benutzer_deaktivieren'=>array(4, 1349)
);
if(fxIsArray($GLOBALS['fxptdata']['backup']) && fxIsArray($GLOBALS['fxptdata']['backup']) && isset($GLOBALS['fxptdata']['backup']['Modus']))
	$OModus=(int)$GLOBALS['fxptdata']['backup']['Modus'];
else
	$OModus=0;
$Modus=$OModus;
$bts='';
if(fxIsArray($_POST))
{
	foreach($modarr as $btn => $bma)
	{
		if(isset($_POST[$btn]))
		{
			$bts=$btn;
			break;
		}
	}
}
if(strlen((string)$bts))
	$Modus=$modarr[$bts][0];

// Set modus title
if(!strlen((string)$bts))
{
	foreach($modarr as $btn => $bma)
	{
		if($Modus === $bma[0])
		{
			$bts=$btn;
			break;
		}
	}
}
$modtitle=get_text($modarr[$bts][1], '15');
$GLOBALS[$bts.'_bp']=true;
//echo('$Modus='.$Modus.', $bts='.$bts.' (field_id='.$modarr[$bts][1].'), $modtitle='.$modtitle.'<hr />');

// Backup modus
$GLOBALS['fxptdata']['backup']=array('Modus'=>$Modus);

// E-Mail an Benutzer senden deaktiviert, deshalb kann auch kein Passwort generiert oder eine Sperre aufgehoben werden, da hierbei E-Mails an den entsprechenden Benutzer gesendet werden würde
if(!check_mail(1738,'MA'))
{
	$Button_Passwort_generieren_nv=true;
	$Button_Sperre_aufheben_nv=true;
}
?>