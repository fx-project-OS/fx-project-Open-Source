<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : projekt_erstellen_val.inc                                    //
// Version     : 24.1                                                         //
// Begin       : 2020-10-29                                                   //
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
 * PF 280: Create Main Project
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 24.1
 */

if(fxIsArray($_POST) && isset($_POST['Button_Multi']))
{
//fxDebug('_POST');
	$float_zero='f'.get_entry(0,'dezimal');

	if(!$err)
	{
		// Prefix
		$pprefix='';
		if(isset($_POST['pprefix']))
			$pprefix=trim((string)$_POST['pprefix']);

		// Abrechnungsart (Personen)
		$abr_p=$GLOBALS['fxpglobals']['settings']['def_abrechnart'];
		if(!$abr_p || ($abr_p == FXP_INV_MAT_HU))
			$abr_p=FXP_INV_HOUR;	// Stundenbasis
		// Abrechnungsart (Material)
		$abr_m=$abr_p;
		if($abr_m == FXP_INV_HOUR)
			$abr_m=FXP_INV_MAT_HU;	// Stunden- oder Mengenbasis

		// Prepare post data
		$psarr=array(
			'Projekt_ID'					=> 'i-666',
			'Mutterprojekt_ID'				=> 'i0',
			'aktionscode'					=> 'i0',

			// Daten-Block
			'Vorgangsnummer'				=> 's'.$_POST['Vorgangsnummer'],
			'Vorgang_naechster'				=> 's'.$_POST['Vorgangsnummer'],
			'Name_Projekt'					=> 's'.$_POST['Name_Projekt'],
			'pprefix'						=> 's'.$pprefix,
			'Elter'							=> 'i0',
			'Abhaengigkeit'					=> 'i0',
			'Name_Meilenstein'				=> 's',
			'Projektart'					=> 'i'.FXP_PRJ_MAIN,	// 59
			'Projekt_Status'				=> 'i'.FXP_PS_PLANNED,	// 297
			'Prioritaet'					=> 'i446',	// Mittel
			'projekt_archiviert'			=> 'i0',
			'Ansprechpartner'				=> 'i'.$_POST['Ansprechpartner'],
			'Projektmanager'				=> 'i'.$_POST['Projektmanager'],
			'Projektleiter'					=> 'i'.$_POST['Projektleiter'],
			'Vertreter'						=> 'i'.$_POST['Vertreter'],
			'MaArt_ID'						=> 'i0',
			'Kategorie_ID'					=> 'i0',
			'Beschreibung'					=> 's',

			// Termin-Block
			'Soll_Beg_Dtm'					=> 'd'.$_POST['Soll_Beg_Dtm'],
			'SollBeginnElt'					=> 'd',
			'Aufwand_Soll'					=> $float_zero,
			'Zeiteinheit_Aufw_S'			=> 'i484',	// Std.
			'Soll_End_Dtm'					=> 'd'.$_POST['Soll_End_Dtm'],
			'SollEndeElt'					=> 'd',
			'Zeiteinheit_max_Zeitaufw'		=> 'i484',	// Std.
			'Deadline'						=> 'd',
			'Zeiteinheit_Soll_Verz'			=> 'i484',	// Std.
			'Zeiteinheit_Aufw_I'			=> 'i484',	// Std.

			// Finanz-Block
			'budgetrahmen_pers_int'			=> 'f'.$_POST['budgetrahmen_pers_int'],
			'budgetrahmen_pers_int_rest'	=> 'f',
			'budgetrahmen_pers_ext'			=> 'f'.$_POST['budgetrahmen_pers_ext'],
			'budgetrahmen_pers_ext_rest'	=> 'f',
			'budgetrahmen_mat_int'			=> 'f'.$_POST['budgetrahmen_mat_int'],
			'budgetrahmen_mat_int_rest'		=> 'f',
			'budgetrahmen_mat_ext'			=> 'f'.$_POST['budgetrahmen_mat_ext'],
			'budgetrahmen_mat_ext_rest'		=> 'f',
			'budgettyp'						=> 'i702',		// Standardbudget
			'Abrechnungsart'				=> 'i'.$abr_p,
			'Abrechnungsart_Mat'			=> 'i'.$abr_m,
			'budget_pers_int_gepl'			=> $float_zero,
			'budget_pers_int_gen'			=> $float_zero,
			'budget_pers_int_akt'			=> $float_zero,
			'budget_pers_int_verpl'			=> $float_zero,
			'budget_pers_int_verbr'			=> $float_zero,
			'budget_pers_int_rest'			=> $float_zero,
			'budget_mat_int_gepl'			=> $float_zero,
			'budget_mat_int_gen'			=> $float_zero,
			'budget_mat_int_akt'			=> $float_zero,
			'budget_mat_int_verpl'			=> $float_zero,
			'budget_mat_int_verbr'			=> $float_zero,
			'budget_mat_int_rest'			=> $float_zero,
			'budget_ges_int_gepl'			=> $float_zero,
			'budget_ges_int_gen'			=> $float_zero,
			'budget_ges_int_akt'			=> $float_zero,
			'budget_ges_int_verpl'			=> $float_zero,
			'budget_ges_int_verbr'			=> $float_zero,
			'budget_ges_int_rest'			=> $float_zero,
			'budget_pers_ext_gepl'			=> $float_zero,
			'budget_pers_ext_gen'			=> $float_zero,
			'budget_pers_ext_akt'			=> $float_zero,
			'budget_pers_ext_verpl'			=> $float_zero,
			'budget_pers_ext_verbr'			=> $float_zero,
			'budget_pers_ext_rest'			=> $float_zero,
			'budget_mat_ext_gepl'			=> $float_zero,
			'budget_mat_ext_gen'			=> $float_zero,
			'budget_mat_ext_akt'			=> $float_zero,
			'budget_mat_ext_verpl'			=> $float_zero,
			'budget_mat_ext_verbr'			=> $float_zero,
			'budget_mat_ext_rest'			=> $float_zero,
			'budget_ges_ext_gepl'			=> $float_zero,
			'budget_ges_ext_gen'			=> $float_zero,
			'budget_ges_ext_akt'			=> $float_zero,
			'budget_ges_ext_verpl'			=> $float_zero,
			'budget_ges_ext_verbr'			=> $float_zero,
			'budget_ges_ext_rest'			=> $float_zero,

			// Fortschritts-Block
			'tage'							=> 's7',
			'zeitpunkt'						=> 'i1790',	// Auf Anforderung (Standard)
			'methode'						=> 'i1784',	// Vergleich Sol-/Ist-Aufwand (Standard)

			// Kostenstammdaten-Block
			'nv_zahl1'						=> $float_zero,
			'nv_zahl2'						=> $float_zero,
			'nv_zahl3'						=> 's',
			'nv_zahl4'						=> 's',
			'nv_text5'						=> 'i0',
			'nv_text6'						=> 'i904',
			'nv_text7'						=> 's00000000000000|99999999999999',
			'kostentraeger'					=> 'i0',
			'kostenstellen'					=> 'i0',

			// Info-Block
			'Ref_nummer'					=> 's',
			'Wiederholungart'				=> 'i0',
			'Anzahl'						=> 's',
			'Anmerkung'						=> 's',
			'AnmerkungIntern'				=> 's'
		);

		$user_form_fields=get_user_form_fields();
		if(fxIsArray($user_form_fields))
		{
			foreach($user_form_fields as $user_field)
				$psarr[$user_field['feldname']]='s';
		}

		$btr=$GLOBALS['fxpglobals']['tr'];

		$GLOBALS['fxpglobals']['tr']=23;
		$tr=$GLOBALS['fxpglobals']['tr'];

		$_POST=array(
			'Button_okay'	=> true,
			'tr'			=> $tr,
			'db_update'		=> '1'
		);
		$maskenfeldwerte=array();
		$dbwerte=array();

		foreach($psarr as $pk => $ptv)
		{
			$pkl=strtolower((string)$pk);
			$typ=substr((string)$ptv,0,1);
			if(strlen((string)$ptv) > 1)
				$pvl=substr((string)$ptv,1);
			else
				$pvl='';

			$_POST[$pk]=$pvl;
			$GLOBALS[$pk]=$pvl;

			if(!strlen((string)$pvl))
				$dbv='NULL';
			else if($typ == 'd')
			{
				$pvl=validate_entry($pvl,$errv,'datum');
				$dbv="'".$pvl."'";
			}
			else if($typ == 'f')
			{
				$pvl=validate_entry($pvl,$errv,'dezimal');
				$dbv=$pvl;
			}
			else if($typ == 's')
				$dbv="'".convert_string($pvl,'todb')."'";
			else
				$dbv=$pvl;

			$maskenfeldwerte[$pkl]=$pvl;
			$dbwerte[$pkl]=$dbv;
		}
//fxDebug('_POST');
//fxDebug($maskenfeldwerte,'$maskenfeldwerte', 3);
//fxDebug($dbwerte,'$dbwerte', 2);

		// Prepare data for includes
		// ...tr23.inc
		$Projekt_da=0;
		// ...prj_a_open_ini.inc
		$vAPID=0;
		$vEPID=0;
		$vPART=FXP_PRJ_MAIN;
		$Projekt_IDopen=0;
		$Projekt_ID=-666;
		$infvn='';
		$pinfo=array();
		$budgetrahmen=get_budgetrahmen($vEPID,$vAPID);
		$pma=array('pers','mat');
		$iea=array('int','ext');
		$mwaehrung=' '.$GLOBALS['fxpglobals']['settings']['waehrung'];
		// ...kst_zuord_ini.inc + kst_zuord_db.inc
		$kr_aktiv=false;

		$err=false;
		$incarr=array('prj_b_auswahl_val','prj_c_daten_ini','prj_fschritt_def_ini','prj_fschritt_def_db','prj_f_info_val','prj_f_info_db');

		// Start db transaction
		$_RBA=true;
		db_rollback('start'); // Transaktionsmarke setzen

		// Execute all includes
		foreach($incarr as $incfile)
		{
			$req_fn=fxf_fn_reqFilename($incfile, 23);
			if(strlen((string)$req_fn))
				require($req_fn);
			// Error: The program $var1 necessary for the program function was not found!
			else
			{
				error_msg(3356,false, '&quot;<b class=red>'.$incfile.'</b>&quot;');
				$err=true;
				break;
			}
		}

		$GLOBALS['fxpglobals']['tr']=$btr;
		$tr=$GLOBALS['fxpglobals']['tr'];

		// End db transaction: success or rollback
//$err=true;
		if($err)
		{
			db_rollback('rollback');
			if(isset($GLOBALS['maske12_nv']))
				unset($GLOBALS['maske12_nv']);
		}
		else
		{
			db_rollback('ok');
			$GLOBALS['maske12_nv']=true;

			// The project has been successfully created!|Next step|Create new main project|Edit: Text Form with Diagrams|Edit: Graphically as Gantt
			$tarr=explode('|', meldung(3357));

			// Success message: The project has been successfully created! Next step
			echo('<br /><b class="s4 bold green">'.$tarr[0].'</b><br /><br /><hr class="fxhr" /><br />'.$tarr[1].':<br /><br />'.$GLOBALS['nl']);

			// Buttons
			$bwidth=400;
			$mpath=$GLOBALS['docpath'].substr('00'.$GLOBALS['fxpglobals']['lang'],-2).'/SCR/';
			$mstyle='width:'.($bwidth-2).'px;height:220px;margin-left:8px;border:1px solid #aaaaaa;border-radius:8px;box-shadow:5px 5px 5px rgba(255,255,255, 0.5) inset, -5px -5px 5px rgba(0,0,0, 0.5) inset, 4px 4px 4px rgba(0,0,0, 0.5);';
			echo('<table border="0" cellpadding="0" cellspacing="8">'.$GLOBALS['nl']);
			echo('	<tr>'.$GLOBALS['nl']);
			// ...Create new main project
			echo('		<td valign="top">'.$GLOBALS['nl']);
			echo('			'.fieldSubmit('newtr',$tarr[2],'sub',$bwidth,false,false,false,'280').'<br /><br />'.$GLOBALS['nl']);
			echo('			'.fieldSubmitImage('newtr',$mpath.'000280_000012.png','',$tarr[2],false,false,$GLOBALS['fxpglobals']['tr'],'',' style="'.$mstyle.'"').$nl);
			echo('		</td>'.$GLOBALS['nl']);
			// ...Edit project: Graphically as Gantt
			if(tr_rechte(189))
			{
				echo('		<td valign="top">'.$GLOBALS['nl']);
				echo('			'.fieldSubmit('newtr',$tarr[4],'sub',$bwidth,false,false,false,'189','pid='.$Projekt_ID.'&aktion='.FXP_CHANGE).'<br /><br />'.$GLOBALS['nl']);
				echo('			'.fieldSubmitImage('newtr',$mpath.'000189_000014.png','',$tarr[4],false,false,'189','pid='.$Projekt_ID.'&aktion='.FXP_CHANGE,' style="'.$mstyle.'"').$nl);
				echo('		</td>'.$GLOBALS['nl']);
			}
			// ...Edit project: Text Form with Diagrams
			if(tr_rechte(23))
			{
				echo('		<td valign="top">'.$GLOBALS['nl']);
				echo('			'.fieldSubmit('newtr',$tarr[3],'sub',$bwidth,false,false,false,'23','gproject='.$Projekt_ID.'&aktion='.FXP_CHANGE).'<br /><br />'.$GLOBALS['nl']);
				echo('			'.fieldSubmitImage('newtr',$mpath.'000023_000031.png','',$tarr[3],false,false,'23','gproject='.$Projekt_ID.'&aktion='.FXP_CHANGE,' style="'.$mstyle.'"').$nl);
				echo('		</td>'.$GLOBALS['nl']);
			}
			echo('	</tr>'.$GLOBALS['nl']);
			echo('</table>'.$GLOBALS['nl']);
			echo('<br />'.$GLOBALS['nl']);
		}
	}
}
?>