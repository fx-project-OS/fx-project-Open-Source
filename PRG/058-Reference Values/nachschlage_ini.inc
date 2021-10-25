<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : nachschlage_ini.inc                                          //
// Version     : 21.2                                                         //
// Begin       : 2020-08-27                                                   //
// Last Change : 2020-07-22                                                   //
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
 * PF 58: Reference Values - Initialization file that also does the validation
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 21.2
 */

$tabwert_strtr=array('<'=>"&lt;", '>'=>"&gt;");

// Hinweistext
$msm=meldung(3343,false);	// Folgende Werte sind erlaubt für dieses Feld:<br /><br /><ul style=padding-left:16px;><li><i>(leer)</i> oder <b>+</b> <i>(Pluszeichen)</i><br />&nbsp;&nbsp;&nbsp;<span class=grey>Es wird der Standardmehrwertsteuersatz verwendet.</span><br /><br /></li><li><b>-</b> <i>(Minuszeichen)</i><br />&nbsp;&nbsp;&nbsp;<span class=grey>Es wird der ermäßigte Mehrwertsteuersatz verwendet.</span><br /><br /></li><li>Wert &gt;= <b>0,00</b><br />&nbsp;&nbsp;&nbsp;<span class=grey>Es wird der eingegebene fixe Mehrwertsteuersatz verwendet.<br /><br /></li></ul>

// Felder vorbelegen
if(fxIsArray($_POST) && isset($_POST['ID_Feld']))
	$ID_Feld=(int)$_POST['ID_Feld'];
else
	$ID_Feld=0;

if(fxIsArray($_POST) && isset($_POST['Sprache']))
	$Sprache=(int)$_POST['Sprache'];
else
	$Sprache=(int)$GLOBALS['fxpglobals']['lang'];

// Einzelbeleg?
$eba=false;
if($ID_Feld == 1291)
	$eba=true;

// Validierung?
$ID_Feld=(int)$ID_Feld;	// komische Sache, aber der Integer wurde irgendwo zu einem String abgeändert???
$ca=array();
if(fxIsArray($_POST) && $_POST['Button_Speichern'] && fxIsArray($_POST['nv_text1']))
{
//fxDebug('_POST');
	// Key des übergebenen Arrays ermitteln = Maskenzähler
	reset($_POST['nv_text1']);
	$pk=key($_POST['nv_text1']);

	// Alle Werte durchgehen
	foreach($_POST['nv_text1'][$pk] as $lc => $lv)
	{
		$val_zeilennummer=$lc;

		// Alte Werte
		$la=explode('<|>',$lv);
//fxDebug($la,$lc.': $lv='.$lv);
		$omq=(int)$la[0];	// Mandanten-ID (Quelle)
		$oma=(int)$la[1];	// Mandanten-ID (Aktuell)
		$oid=(int)$la[2];	// Wertetab-ID (0 bei Neu)
		$ops=(int)$la[3];	// Position
		$owt=trim($la[4]);	// Tabwert
		$oms=trim($la[5]);	// Mehrwertsteuer-Satz (nur bei Einzelbelegen)
		$otw=$owt;			// Tabwert (für Wertetabtabelle)
		if(strlen($oms))
			$otw .= '^'.$oms;
//echo('<b>Old</b> $omq='.$omq.', $oma='.$oma.', $oid='.$oid.', $ops='.$ops.', $owt='.$owt.', $oms='.$oms.' -> $otw='.$otw.'<br />');

		// Neue Werte
		$amq=$omq;
		$ama=$oma;
		$aid=$oid;
		$aps=$ops;
		$awt=$owt;
		$ams=$oms;
		$atw=$otw;

		$vl=0;
		if(($aktion == FXP_CREATE) && !$oid && fxIsArray($_POST['Position'][$pk]) && fxIsArray($_POST['TabWert'][$pk]))
			$vl=FXP_CREATE;
		else if(($aktion == FXP_CHANGE) && $oid && fxIsArray($_POST['Position'][$pk]) && fxIsArray($_POST['TabWert'][$pk]))
			$vl=FXP_CHANGE;
		else if(($aktion == FXP_DELETE) && $oid && fxIsArray($_POST['Checkbox'][$pk]) && $_POST['Checkbox'][$pk][$lc])
			$vl=FXP_DELETE;

		// Validate?
//function validate_entry($entry, &$err, $feldarray='text', $muss='0', $nachkomma='2', $trennzeichen='')
		if($vl)
		{
			$lerr=false;

			if($vl != FXP_DELETE)
			{
				// ...Position
				$_POST['Position'][$pk][$lc]=validate_entry($_POST['Position'][$pk][$lc], $errz, 'ganzzahl');
				if($errz)
				{
					$aps=0;
					error_msg(755, false, 'Position'); // Bitte geben Sie in das Feld $var1 eine Zahl ein!
					$lerr=true;
				}
				else
					$aps=(int)$_POST['Position'][$pk][$lc];

				// MwSt.
				if($eba)
				{
					$ams=trim($_POST['MwSt'][$pk][$lc]);
					if(strlen($ams) && ($ams != '+') && ($ams != '-'))
					{
						$_POST['MwSt'][$pk][$lc]=validate_entry($_POST['MwSt'][$pk][$lc], $errd, 'dezimal', '1');
						if($errd)
						{
							error_msg(3338, array('MwSt'), '<b>'.get_text(71,'5').'</b>', '<br />'.$msm); // Das Feld "$var1" enthält einen ungültigen Wert!$var2
							$lerr=true;
						}
						else
							$ams=get_double($_POST['MwSt'][$pk][$lc]);
					}
					else
						$_POST['MwSt'][$pk][$lc]='^'.$ams;
				}
				else
					$ams='';

				// ...Wert
				$awt=trim($_POST['TabWert'][$pk][$lc]);
				if(($aps || strlen($ams)) && !strlen($awt))
				{
					error_msg(749, false, 'TabWert'); // Das Feld $var1 darf nicht leer sein. Bitte geben Sie einen Text ein!
					$lerr=true;
				}

				$atw=$awt;
				if(strlen($ams))
					$atw .= '^'.$ams;
//echo('<b>New</b> $amq='.$amq.', $ama='.$ama.', $aid='.$aid.', $aps='.$aps.', $awt='.$awt.', $ams='.$ams.' -> $atw='.$atw.'<br />');
			}
			else
				$atw='';

			// Validierung OK?
			if(!$lerr)
			{
				// ...Neuanlage
				if(!$aid && strlen($atw))
					$ca[]=array('wertetabid'=>$aid, 'position'=>$aps, 'tabwert'=>$atw, 'wert'=>$awt);
				// ...Löschen
				else if($aid && !strlen($atw))
					$ca[]=array('wertetabid'=>$aid, 'position'=>$aps, 'tabwert'=>$atw, 'wert'=>$otw);
				// ...Ändern
				else if($aid && ($aps != $ops) || ($atw != $otw))
					$ca[]=array('wertetabid'=>$aid, 'position'=>$aps, 'tabwert'=>$atw, 'wert'=>$awt);
			}
			else
				$err=true;
		}
	}
//$err=true;
//fxDebug('_POST');
}
//fxDebug($ca,'$ca');
?>