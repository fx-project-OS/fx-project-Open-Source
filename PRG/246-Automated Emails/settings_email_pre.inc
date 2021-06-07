<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : settings_email_pre.inc                                       //
// Version     : 21.1                                                         //
// Begin       : 2020-10-14                                                   //
// Last Change : 2020-10-14                                                   //
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
 * PF 246: Automated Emails - Mask Values
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 21.1
 */

if(!isset($index))
	$index=true;

$gls=array();
if(fxIsArray($pgr) && fxIsArray($bit) && $cs)
{
	// Globale Settings
	$sql="SELECT kategorie_id, wertetabid, feld, aktionscode FROM settings_email WHERE mandanten_id=0 AND kategorie_id>1 ORDER BY position";
	$gls=db_values($sql, '*');
fxDebug($gls, $sql, 3);
}

// Liste anzeigen
if(fxIsArray($gls))
{
	// Alle Spalten
	$fa=array('Einstellungs_Name');
	for($c=0; $c<$cs; $c++)
		$fa[]='nv_check'.$c;

	$k=0;
	$z=0;
	$tc=0;
	$mc=$GLOBALS['_maskcounter'];
	foreach($gls as $g)
	{
		// Aktuelle Einstellung
		$aes=$GLOBALS['fxpglobals']['settings']['ems'][$g['wertetabid']];
fxDebug($aes, 'Akt. Einstellung', 2);

		// Kategorie
		if($g['kategorie_id'] != $k)
		{
			// Leerzeile mit Rahmen?
			if($z)
			{
				fxf_maskColumnBorder($fa, 1, -1, $mc, $z, true);
				$z++;
			}

			$k=$g['kategorie_id'];
			$txt='&nbsp;'.meldung($k, true);
			fxf_listSectHeader($mc, $z, $txt, 'einstellungs_name', 'nv_check'.($cs-1), 1+$cs);

			$tc++;
			$z++;
		}

		// Einstellung
		$cls='black';
		if($g['aktionscode'] != $aes['ac'])
			$cls='blue';
		$add='';
		if($GLOBALS['__DGBLVL'])
			$add='<i class="lightgrey">'.$g['wertetabid'].'</i>';
		$GLOBALS['fxptdata']['lvalues'][$mc][$z]['einstellungs_name']='~<table><tr><td valign=top>'.$add.'</td><td><font class="'.$cls.'">'.text_wrap(meldung($g['wertetabid'], true), true, 88).'</font></td></tr></table>';

		// Checkboxen
		$f=$g['feld'];
		$a=$aes['ac'];
		for($c=0; $c<$cs; $c++)
		{
			$b=$bit[$c];
			$p=$pgr[$b];

			if($f & $b)	// Checkbox darf verändert werden
			{
				if(($err && $_POST['nv_check'.$c][$mc][$z]) || (!$err && ($a & $b)))
					$ct=1;
				else
					$ct='';
			}
			else
				$ct='~<font class="lightgrey">&middot;</font>';
			$GLOBALS['fxptdata']['lvalues'][$mc][$z]['nv_check'.$c]=$ct;
		}

		// Hidden
		$GLOBALS['fxptdata']['lvalues'][$mc][$z]['kategorie_id']=$g['kategorie_id'];
		$GLOBALS['fxptdata']['lvalues'][$mc][$z]['wertetabid']=$g['wertetabid'];
		$GLOBALS['fxptdata']['lvalues'][$mc][$z]['feld']=$g['feld'];

		$z++;
	}

	// Leerzeile mit Rahmen?
	if($z)
	{
		fxf_maskColumnBorder($fa, 1, -1, $mc, $z, true);
		$z++;
	}
}
?>