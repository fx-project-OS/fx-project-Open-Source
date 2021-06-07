<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : projekt_vernichten_pre.inc                                   //
// Version     : 21.1                                                         //
// Begin       : 2020-12-16                                                   //
// Last Change : 2020-12-16                                                   //
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
 * PF 234: Destroy Projects
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 21.1
 */

if(sizeof($pinf))
{
	$mc=$GLOBALS['_maskcounter'];
	$z=0;

	// Hinweis + Projekte/Aufgaben
	$txt  = '<div class="s4 red" style="margin:8px;padding:8px;border:1px solid #e1001a;border-radius:6px;background:#fff0f4;text-align:center;">'.meldung(434, false).'</div>';
	$txt .= '<div style="margin:8px;padding:8px;"><table border=0 cellpadding=0 cellspacing=0>';
	$txt .= '<tr><td colspan="3">&nbsp;<img src="'.$GLOBALS['gfxpath'].'mt_st.png" align="bottom"></td></tr>';
	$txt .= '<tr><td>&nbsp;<img src="'.$GLOBALS['gfxpath'].'mt_l1.png" align="top">&nbsp;</td><td><b class="grey"># '.meldung(582, false).':</b></td><td class="red" nowrap align="right">&nbsp;&nbsp;'.$pinf['sub'].'</td></tr>';
	$txt .= '<tr><td>&nbsp;<img src="'.$GLOBALS['gfxpath'].'mt_l2.png" align="top">&nbsp;</td><td><b class="darkgrey"># '.meldung(621, false).':</b></td><td class="red" nowrap align="right">&nbsp;&nbsp;'.$pinf['tsk'].'</td></tr>';
	$txt .= '</table></div>';
	$GLOBALS['fxptdata']['lvalues'][$mc][$z]['checkbox']='~|';
	fxf_listSectHeader($mc, $z, $txt, 'tabelle', 'nummer', 3, -2, 0);
	$z++;

	// Tabellen
	if(fxIsArray($pinf['tab']))
	{
		// Line height
		$Tabelle_height=20;

		// No right border in the following column
		fxf_maskColumnBorder(array('Checkbox'), 0, -1, $mc);

		foreach($pinf['tab'] as $tabelle => $tdata)
		{
			$lcls='^class_rv'.(($z+1)%2).'|';

			if(!$tdata['cnt'])
				$cls='grey';
			else if($tdata['muss'] == 1)	// Wichtig
				$cls='red';
			else if($tdata['muss'] == 2)	// Auswahl
				$cls='blue';
			else
				$cls='black';

			// Checkbox
			if(!$tdata['cnt'])
				$GLOBALS['fxptdata']['lvalues'][$mc][$z]['checkbox']='~<img src="'.$GLOBALS['gfxpath'].'0.png">';
			else if($tdata['muss'] < 2)
				$GLOBALS['fxptdata']['lvalues'][$mc][$z]['checkbox']='~<img src="'.$GLOBALS['gfxpath'].'1.png">';
			else
			{
				$cbname='cb_'.$tabelle;
				if($_POST[$cbname])
					$checked='checked';
				else
					$checked='';
				$GLOBALS['fxptdata']['lvalues'][$mc][$z]['checkbox']='~'.fieldCheckbox($cbname, '', $checked);
			}

			// Tabelle
			$GLOBALS['fxptdata']['lvalues'][$mc][$z]['tabelle']='~&nbsp;<font class="'.$cls.'">'.strtoupper($tabelle).'</font>&nbsp;';

			// Textzeile
			if(substr($tabelle,-8) == '_history')
			{
				if($GLOBALS['fxpglobals']['lang'] == 1)
					$txt='Alle Neuanlagen, &Auml;nderungen oder L&ouml;schungen in der Tabelle &quot;'.$tabelle.'&quot;';
				else
					$txt='All creations, changes or deletions in table &quot;'.$tabelle.'&quot;';
				$GLOBALS['fxptdata']['lvalues'][$mc][$z]['textzeile']	= '~&nbsp;<font class="'.$cls.'">['.$txt.']</font>&nbsp;';
			}
			else
			{
				$tsql="SELECT t.id_tabellen, x.text_kurz FROM tabellen t, texte x WHERE {where} AND x.id_referenz=t.id_tabellen AND x.id_ref_art=1 AND x.id_sprache=".$GLOBALS['fxpglobals']['lang'];

				$sql=str_replace('{where}', db_like('t.tabname', $tabelle), $tsql);
				$tar=db_values($sql);
				if(!fxIsArray($tar))
				{
					$lup=strrpos($tabelle,'_');
					if($lup)
					{
						$stabelle=substr($tabelle,0,$lup);
						$sql=str_replace('{where}', db_like('t.tabname', $stabelle), $tsql);
						$tar=db_values($sql);
					}
				}
				if(fxIsArray($tar))
				{
					if($tar['text_kurz'])
						$GLOBALS['fxptdata']['lvalues'][$mc][$z]['textzeile']	= '~&nbsp;<font class="'.$cls.'">'.$tar['text_kurz'].'</font>&nbsp;';
					else
						$GLOBALS['fxptdata']['lvalues'][$mc][$z]['textzeile']	= '~&nbsp;<font class="'.$cls.'">['.$tar['id_tabellen'].': '.$tabelle.']</font>&nbsp;';
				}
				else
					$GLOBALS['fxptdata']['lvalues'][$mc][$z]['textzeile']	= '~&nbsp;<font class="'.$cls.'">[? <i>'.$tabelle.'</i> ?]</font>&nbsp;';
			}

			// Anzahl
			$anz=$tdata['cnt'];
			if($anz > 0)
				$anz='<b>'.$anz.'</b>';
			else
				$anz='<font class="lightgrey">'.$anz.'</font>';
			$GLOBALS['fxptdata']['lvalues'][$mc][$z]['nummer']='~&nbsp;'.$anz.'&nbsp;';

			$z++;
		}

		// Last line with Top Border?
		if($z)
		{
			$fa=fxf_getMaskFields(475,false,array('Checkbox'=>0));
			fxf_maskColumnBorder($fa, 1, -1, $mc, $z, true);
			$z++;
		}
	}
}
?>