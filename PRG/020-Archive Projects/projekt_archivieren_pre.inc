<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : projekt_archivieren_pre.inc                                  //
// Version     : 21.2                                                         //
// Begin       : 2020-12-15                                                   //
// Last Change : 2020-12-15                                                   //
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
 * PF 20: Archive Projects
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 21.2
 */

if($mp_rights && fxIsArray($mp_list))
{
	// Listenwerteindex verwenden
	$index=true;

	// Projekt-Status
	$status=fxf_getReferenceValues('projekt_status');

	// Tooltip: Dieses Projekt kann nicht archiviert werden, da es noch nicht abgeschlossen wurde!
	$tt_text=' tooltip="'.meldung(2869, true).'" style="cursor:help;"';
	$tt_icon='<img src="'.$GLOBALS['gfxpath'].'tts.png" align="top" hspace="4">';

	// Maskenvariablen
	$mc=$GLOBALS['_maskcounter'];
	$z=0;

	// Liste der gefunden Projekte durchgehen
	foreach($mp_list as $mp)
	{
		// Steuerungsvariablen
		$cls='black';
		$tt=false;
		if($mp['projekt_status'] != FXP_PS_COMPLETED)
		{
			$cls='grey';
			$tt=true;
		}
		else if($mp['projekt_archiviert'])
			$cls='red';

		// Name des Projektes [name_projekt]
		$GLOBALS['fxptdata']['lvalues'][$mc][$z]['name_projekt']='~<span';
		if($tt)
			$GLOBALS['fxptdata']['lvalues'][$mc][$z]['name_projekt'] .= $tt_text;
		$GLOBALS['fxptdata']['lvalues'][$mc][$z]['name_projekt'] .= '><font class="'.$cls.'"><b>';
		if($sort_number)
			$GLOBALS['fxptdata']['lvalues'][$mc][$z]['name_projekt'] .= no_null($mp['vorgangsnummer']).'</b>&nbsp;&nbsp;'.$mp['name_projekt'];
		else
			$GLOBALS['fxptdata']['lvalues'][$mc][$z]['name_projekt'] .= $mp['name_projekt'].'</b>&nbsp;&nbsp;['.no_null($mp['vorgangsnummer']).']';
		$GLOBALS['fxptdata']['lvalues'][$mc][$z]['name_projekt'] .= '</font>';
		if($tt)
			$GLOBALS['fxptdata']['lvalues'][$mc][$z]['name_projekt'] .= $tt_icon;
		$GLOBALS['fxptdata']['lvalues'][$mc][$z]['name_projekt'] .= '</span>';

		// Status [projekt_status]
		$GLOBALS['fxptdata']['lvalues'][$mc][$z]['projekt_status']='~<font class="'.$cls.'">'.$status[$mp['projekt_status']].'</font>';

		// Gep. Start [soll_beg_dtm]
		$GLOBALS['fxptdata']['lvalues'][$mc][$z]['soll_beg_dtm']='~<font class="'.$cls.'">'.get_entry($mp['soll_beginn'], "datum").'</font>';

		// Gep. Ende [soll_end_dtm]
		$GLOBALS['fxptdata']['lvalues'][$mc][$z]['soll_end_dtm']='~<font class="'.$cls.'">'.get_entry($mp['soll_ende'], "datum").'</font>';

		// Tat. Start [ist_beg_dtm]
		$val='';
		if(strlen($mp['ist_beginn']))
			$val=get_entry($mp['ist_beginn'], "datum");
		$GLOBALS['fxptdata']['lvalues'][$mc][$z]['ist_beg_dtm']='~<font class="'.$cls.'">'.$val.'</font>';

		// Tat. Ende [ist_end_dtm]
		$val='';
		if(strlen($mp['ist_ende']))
			$val=get_entry($mp['ist_ende'], "datum");
		$GLOBALS['fxptdata']['lvalues'][$mc][$z]['ist_end_dtm']='~<font class="'.$cls.'">'.$val.'</font>';

		// Archiviert [projekt_archiviert]
		if($mp['projekt_status'] != FXP_PS_COMPLETED)	// Nur abgeschlossene Projekte können archiviert werden
		{
			${'projekt_archiviert'.$z.'_dis'}=true;
			$val='';
		}
		else if($err)
			$val=$_POST['projekt_archiviert'][$mc][$z];
		else if($mp['projekt_archiviert'])
			$val='1';
		else
			$val='';
		$GLOBALS['fxptdata']['lvalues'][$mc][$z]['projekt_archiviert']=$val;

		// Hidden: [projekt_id]
		$GLOBALS['fxptdata']['lvalues'][$mc][$z]['projekt_id']=$mp['projekt_id'];

		$z++;
	}

	// Letzte Zeile mit Top Border?
	if($z)
	{
		$fa=fxf_getMaskFields(536);
		fxf_maskColumnBorder($fa, 1, -1, $mc, $z, true);
		$z++;
	}
}
?>