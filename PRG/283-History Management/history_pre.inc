<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : history_pre.inc                                              //
// Version     : 24.1                                                         //
// Begin       : 2022-04-20                                                   //
// Last Change : 2024-10-09                                                   //
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
 * PF 283: History Management
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 24.1
 */

$index=true;

// Checkbox always enabled
$Checkbox_sf=true;

// Mask counter
$mc=$GLOBALS['_maskcounter'];

// NoWrap fields
$Tabelle_nw=true;
$Jahr_nw=true;
$Dateiname_nw=true;
$version_nr_nw=true;
$Datum_nw=true;
$anz_nw=true;
$dateigroesse_nw=true;

// Literals
$bla=explode('|', meldung(3386));	// Purge Year|Purge Earliest Year|Restore Selected Year|Restore Year|Delete Selected Years|Delete Year|Latest valid Backup|Earlier valid Backup
//fxDebug($bla,'$bla', 0);
$lit_year=get_text(286,'10');		// Year
$lit_table=get_text(1440,'15');		// Table
$lit_number=get_text(1571,'10');	// Number
$lit_total=meldung(1555);			// Grand-total
$lit_error=meldung(2029);			// ERROR

// Tooltip texts
$tha=explode('|', meldung(3396));	// Listing of tables|Listing of Client tables|Listing of History tables
$tm=meldung(3358,false);			// <b>&gt; $var1 additional entries exist &lt;</b><br /><i class=lightgrey>(Click to display the complete list in a new popup)</i>

// Error infos
$hea=explode('|', meldung(3389));	// Info missing|Header entry missing|Wrong header client|Wrong header year|Wrong header date|Wrong header version|Wrong header checksum|Wrong info size|Wrong info year|Wrong info client|Wrong info date|Wrong info version|Wrong info database name|Wrong info database type|Wrong info checksum
//fxDebug($hea,'$hea', 0);

// Mode: Create
if($GLOBALS['fxpglobals']['action'] == FXP_CREATE)
{
	// Get all history tables
	$hta=db_table('%_history');
//fxDebug($hta,'$hta', 0);

	// Get amount of yearly entries from all history tables
	$haa=array();
	if(fxIsArray($_GET) && isset($_GET['ssm']) && ($_GET['ssm'] === '1'))
		$ly=(int)substr((string)$GLOBALS['date'],0,4);
	else
		$ly=(int)substr((string)$GLOBALS['date'],0,4)-1;
	$ey=0;
	foreach($hta as $tn => $dummy)
	{
		$tn=strtolower((string)$tn);

		// Exceptions
		$exs=fxHistoryTableExceptions($tn);

		$sql="SELECT MIN(zeitstempel) as ets FROM ".$tn." WHERE mandanten_id=".$GLOBALS['fxpglobals']['client'].$exs;
		$ety=db_value($sql);
//fxDebug($ety,$sql, 0);
		if(!is_null($ety) && strlen((string)$ety))
		{
			$ety=(int)substr((string)$ety,0,4);
			if(!$ey)
				$ey=$ety;
			else
				$ey=min($ey,$ety);
			for($y=$ety; $y<=$ly; $y++)
			{
				$sql="SELECT COUNT(zeitstempel) as ye FROM ".$tn." WHERE zeitstempel LIKE '".$y."%' AND mandanten_id=".$GLOBALS['fxpglobals']['client'].$exs;
				$ye=(int)db_value($sql);
//fxDebug($ye,$sql.'<br />$ety='.$ety.', $ly='.$ly, 0);
				if($ye)
				{
					if(!isset($haa[$y]))
						$haa[$y]=array('amount'=>0, 'tables'=>array());
					$haa[$y]['amount'] += $ye;
					$haa[$y]['tables'][$tn]=$ye;
				}
			}
		}
	}
//fxDebug($haa,'$haa: $ly='.$ly.', $ey='.$ey, 0);

	// List years
	if(sizeof($haa))
	{
		ksort($haa);
		$GLOBALS['fxptdata']['lvalues'][$mc]=array();
		$ln=0;

		// Hide columns
		$Checkbox_nv=true;
		$Jahr_nv=true;
		$Dateiname_nv=true;
		$version_nr_nv=true;
		$Datum_nv=true;
		$dateigroesse_nv=true;

		// Display class for Grand-total
		$ta_class='^class_cr3" nowrap style="height:22px;border-top:1px solid #'.$GLOBALS['msk_nbcol'].';border-right:1px solid #'.$GLOBALS['msk_tbcol'].';|~&nbsp;';

		// Mask fields
		$fa=fxf_getMaskFields(549,false,array('Checkbox'=>0, 'Jahr'=>0, 'Dateiname'=>0, 'version_nr'=>0, 'Datum'=>0, 'dateigroesse'=>0, 'link'=>0));
//fxDebug($fa,'$fa', 0);

		// No [Delete] button
		$Button_Loeschen_nv=true;

		// Rename [Save] button to: Purge Earliest Year
		$Button_Speichern_btxt=$bla[1].': '.$ey;

		foreach($haa as $year => $ha)
		{
			// Year
			$ycol='black';
			if($year >= $ly)
				$ycol='grey';
			else if($year <= $ly-9)
				$ycol='red';
			$GLOBALS['fxptdata']['lvalues'][$mc][$ln]['link']='~|';
			fxf_listSectHeader($mc, $ln, '&nbsp;<font class="s4b '.$ycol.'">'.$lit_year.' '.$year.'</font>', 'tabelle', 'anz', 2, -2, 1);
			$ln++;

			// Table names
			foreach($ha['tables'] as $tablename => $amount)
			{
				$GLOBALS['fxptdata']['lvalues'][$mc][$ln]['tabelle']='~'.$tablename;
				$GLOBALS['fxptdata']['lvalues'][$mc][$ln]['anz']='~'.get_entry($amount,'integer_sp');
				$GLOBALS['fxptdata']['lvalues'][$mc][$ln]['link']='~|';
				// Hidden
				$GLOBALS['fxptdata']['lvalues'][$mc][$ln]['nv_text1']=$year.'|'.$tablename.'|'.$amount;
				$ln++;
			}

			// Yearly grand-total
			fxf_maskColumnBorder($fa, 3, -1, $mc, $ln, true);
			$GLOBALS['fxptdata']['lvalues'][$mc][$ln]['tabelle']=$ta_class.'<b>'.$lit_total.'</b> <i>('.$lit_year.' '.$year.')</i>';
			$GLOBALS['fxptdata']['lvalues'][$mc][$ln]['anz']=$ta_class.'<b>'.get_entry($ha['amount'],'integer_sp').'</b>&nbsp;';
			// ...Button: Purge Year
//function fieldSubmit($fieldname, $value, $type='sub', $width=0, $dis=false, $nva=false, $force=false, $ltr='', $gparam='', $jsfunc='', $style='', $form='')
			$btn=fieldSubmit('purge_'.$year, $bla[0].': '.$year);
			$GLOBALS['fxptdata']['lvalues'][$mc][$ln]['link']='~'.$btn;
			$ln++;

			// Empty line with top border
			$GLOBALS['msk_nbcol_549_'.$ln]=$GLOBALS['msk_tbcol'];
			fxf_maskColumnBorder($fa, 1, -1, $mc, $ln, true);
			$ln++;
		}
	}
}
// Mode: Display, Change or Delete
else
{
	$hba=array();

	// Search for existing backups
	if(file_exists($cfa['cdmspath']) && is_dir($cfa['cdmspath']))
	{
		$srch='/_fxp_history_[0-9]{4}_[0-9]{12}_[0-9]{9}.*\.txt$/';	// _fxp_history_{Year: YYYY}_{Datetime: YYYYMMDDHHII}_{Version: VVVVVVVVV}*.txt
		$sa=fxScanDir($cfa['cdmspath'], false, true, $srch);
//fxDebug($sa,'$sa: '.$cfa['cdmspath'].' with search pattern '.$srch, 0);
		if($sa['file_entries'] > 0)
		{
			foreach($sa['files'] as $filename => $filepath)
			{
				$hby=(int)substr((string)$filename,13,4);
				$hbd=substr((string)$filename,18,12);
				$hbv=substr((string)$filename,31,9);
				$hfs=filesize($filepath);
				$hba[$filename]=array(
					'filepath'=>$filepath, 'filename'=>$filename, 'year'=>$hby, 'datetime'=>$hbd, 'version'=>$hbv, 'filesize'=>$hfs, 'amount'=>0,
					'latest'=>false, 'error'=>'',
					'headers'=>array('filename'=>'', 'versioncode'=>'', 'date'=>'', 'checksum'=>''),
					'info'=>array('year'=>'', 'tables'=>'')
				);

				// Analyze history backup text file
//echo('<b>READ ['.$filepath.']</b>:<br />');
				$hfc='';
				$hro=0;
				$hct=0;
				$hal=0;
				while(true)
				{
					$hrl=min(1024,$hfs-$hro);
					$hrs=file_get_contents($filepath, false, NULL, $hro, $hrl);
//echo('&middot; '.$hct.': '.$hro.' ('.$hrl.', '.strlen((string)$hrs).')<br />');
					if(($hrs === false) || !strlen((string)$hrs))
						break;
					else
					{
						$hfc .= $hrs;
						$hro += $hrl;
						$hfa=explode("\n", $hfc);
						$has=sizeof($hfa);
						while($hal < $has-1)
						{
							$hln=trim((string)$hfa[$hal]);
							if(strlen((string)$hln) && (substr((string)$hln,0,3) != '///'))
							{
								if(substr((string)$hln,0,8) == '*H|'.$hby.'|')	// History info: Year
								{
//echo('&rarr; Line '.$hal.': <b>Add INFO</b> [year]<br />');
									$hba[$filename]['info']['year']=substr((string)$hln,3);
								}
								else if(substr((string)$hln,0,3) == '*H|')		// History info: Tables?
								{
//echo('&rarr; Line '.$hal.': <b>Add INFO</b> [tables]<br />');
									$hba[$filename]['info']['tables']=substr((string)$hln,3);
								}
								else
								{
									$hcp=strpos($hln,':');
									if($hcp)
									{
										$hlt=strtolower((string)str_replace(' ', '', trim(substr((string)$hln,3,$hcp-3))));
										if(isset($hba[$filename]['headers'][$hlt]))
										{
//echo('&rarr; Line '.$hal.': <b>Add HEADER</b> ['.$hlt.']<br />');
											$hba[$filename]['headers'][$hlt]=trim(substr((string)$hln,$hcp+1, -3));
										}
										else
										{
//echo('&larr; Line '.$hal.': <i class=grey>Skip HEADER ['.$hlt.']</i><br />');
										}
									}
									else
									{
//echo('&larr; Line '.$hal.': <i class=grey>Skip HEADER</i><br />');
									}
								}
							}
							else
							{
//echo('&larr; Line '.$hal.': <i class=grey>Skip</i><br />');
							}
							$hal++;
						}
					}

					if(strlen((string)$hba[$filename]['info']['year']) && strlen((string)$hba[$filename]['info']['tables']))
						break;

					$hct++;
				}
//echo('<hr />');

				// Do we have all necessary entries?
				if(!strlen((string)$hba[$filename]['info']['year']) || !strlen((string)$hba[$filename]['info']['tables']))
					$hba[$filename]['error'] .= '<li>'.$hea[0].'</li>';	// Info missing
				if(!strlen((string)$hba[$filename]['headers']['filename']) || !strlen((string)$hba[$filename]['headers']['versioncode']) || !strlen((string)$hba[$filename]['headers']['date']) || !strlen((string)$hba[$filename]['headers']['checksum']))
					$hba[$filename]['error'] .= '<li>'.$hea[1].'</li>';	// Header entry missing
				if(!strlen((string)$hba[$filename]['error']))
				{
					$hfe=explode('-', substr((string)$hba[$filename]['headers']['filename'],0,-4));
//fxDebug($hfe,'$hfe', 0);
					// Wrong header client?
					$hfc=-1;
					if(sizeof($hfe) > 1)
						$hfc=(int)$hfe[1];
					if($hfc != $GLOBALS['fxpglobals']['client'])
						$hba[$filename]['error'] .= '<li>'.$hea[2].'</li>';
					// Wrong header year?
					$hfy=-1;
					if(sizeof($hfe) > 2)
						$hfy=(int)$hfe[2];
					if($hfy != $hba[$filename]['year'])
						$hba[$filename]['error'] .= '<li>'.$hea[3].'</li>';
					// Wrong header date?
					if(substr((string)$hba[$filename]['datetime'],0,8) != str_replace('-', '', $hba[$filename]['headers']['date']))
						$hba[$filename]['error'] .= '<li>'.$hea[4].'</li>';
					// Wrong header version?
					if($hba[$filename]['version'] != $hba[$filename]['headers']['versioncode'])
						$hba[$filename]['error'] .= '<li>'.$hea[5].'</li>';
					// Wrong header checksum?
					$chk=lineCode($hba[$filename]['headers']['filename'],'HISTORY');
					if($chk != $hba[$filename]['headers']['checksum'])
						$hba[$filename]['error'] .= '<li>'.$hea[6].'</li>';

					$hia=explode('|', $hba[$filename]['info']['year']);
//fxDebug($hia,'$hia: '. $hba[$filename]['info']['year'], 0);
					if(sizeof($hia) < 9)
						$hba[$filename]['error'] .= '<li>'.$hea[7].'</li>';	// Wrong info size
					else
					{
						$hba[$filename]['amount']=(int)$hia[2];

						// Wrong info year?
						if((int)$hia[0] != $hba[$filename]['year'])
							$hba[$filename]['error'] .= '<li>'.$hea[8].'</li>';
						// Wrong info client?
						if((int)$hia[1] != $GLOBALS['fxpglobals']['client'])
							$hba[$filename]['error'] .= '<li>'.$hea[9].'</li>';
						// Wrong info date?
						if(substr((string)$hba[$filename]['datetime'],0,8) != substr((string)$hia[3],0,8))
							$hba[$filename]['error'] .= '<li>'.$hea[10].'</li>';
						// Wrong info version?
						if($hba[$filename]['version'] != $hia[4])
							$hba[$filename]['error'] .= '<li>'.$hea[11].'</li>';
						// Wrong info database name?
						if($hia[6] != $GLOBALS['fxpglobals']['dbparam']['dbname'])
							$hba[$filename]['error'] .= '<li>'.$hea[12].'</li>';
						// Wrong info database type?
						if((int)$hia[7] != $GLOBALS['fxpglobals']['dbparam']['dbtype'])
							$hba[$filename]['error'] .= '<li>'.$hea[13].'</li>';
						// Wrong info checksum?
						$chk=lineCode($hia[0].'|'.$hia[1].'|'.$hia[2].'|'.$hia[3].'|'.$hia[4].'|'.$hia[5].'|'.$hia[6].'|'.$hia[7], 'HI-'.$hia[1].'-'.$hia[0]);
						if($chk != $hia[8])
							$hba[$filename]['error'] .= '<li>'.$hea[14].'</li>';
					}
				}
			}
		}
	}

	// Mark latest valid year entry
	if(sizeof($hba))
	{
		$lvya=array();
		foreach($hba as $filename => $ba)
		{
			if(!strlen((string)$ba['error']))
				$lvya[$ba['year']]=$filename;
		}
//fxDebug($lvya,'$lvya', 0);
		if(sizeof($lvya))
		{
			foreach($lvya as $filename)
				$hba[$filename]['latest']=true;
		}
	}
//fxDebug($hba,'$hba: '.$cfa['cdmspath'], 0);

	// Display existing backups
	if(sizeof($hba))
	{
		ksort($hba, SORT_STRING);

		$maske549_trheight=32;
		$GLOBALS['fxptdata']['lvalues'][$mc]=array();
		$ln=0;

		// Hide columns
		if($GLOBALS['fxpglobals']['action'] == FXP_DISPLAY)
			$Checkbox_nv=true;
		$Tabelle_nv=true;

		// Mask fields
		$fa=fxf_getMaskFields(549,false,array('Checkbox'=>0, 'Tabelle'=>0, 'link'=>0));
//fxDebug($fa,'$fa', 0);

		// Display [Save] button only in Change mode and rename it to: Restore Selected Years
		if($GLOBALS['fxpglobals']['action'] != FXP_CHANGE)
			$Button_Speichern_nv=true;
		else
			$Button_Speichern_btxt=$bla[2];

		// Display [Delete] button only in Delete mode and rename it to: Delete Selected Years
		if($GLOBALS['fxpglobals']['action'] != FXP_DELETE)
			$Button_Loeschen_nv=true;
		else
			$Button_Loeschen_btxt=$bla[4];

		foreach($hba as $filename => $ba)
		{
			fxf_maskColumnBorder(array('Checkbox', 'link'), 0, -1, $mc, $ln, true);

			$col='888888';
			if(strlen((string)$ba['error']))
				$col='e1001a';
			else if($ba['latest'])
				$col='000000';

			// (Checkbox)
			if($GLOBALS['fxpglobals']['action'] != FXP_DISPLAY)
			{
				if(strlen((string)$ba['error']) && ($GLOBALS['fxpglobals']['action'] != FXP_DELETE))
					$GLOBALS['fxptdata']['lvalues'][$mc][$ln]['checkbox']='~|';
				else
				{
					$cb=0;
					// Error?
					if($err && fxIsArray($fna))
					{
						foreach($fna as $cf => $cs)
						{
							if(($cf == $ba['filename']) && (($cs&1) == 1))
							{
								$cb=1;
								break;
							}
						}
					}
//function fieldSubmit($fieldname, $value, $type='sub', $width=0, $dis=false, $nva=false, $force=false, $ltr='', $gparam='', $jsfunc='', $style='', $form='')
					$GLOBALS['fxptdata']['lvalues'][$mc][$ln]['checkbox']='^class_|'.$cb;
				}
			}
			// Year
			$GLOBALS['fxptdata']['lvalues'][$mc][$ln]['jahr']='~<b style="color:#'.$col.';">'.$ba['year'].'</b>';
			// File Name
			$GLOBALS['fxptdata']['lvalues'][$mc][$ln]['dateiname']='~<span style="color:#'.$col.';">'.$ba['filename'].'</span>';
			// Version
			$vrs=(int)substr((string)$ba['version'],0,3).'.'.(int)substr((string)$ba['version'],3,3).'.'.(int)substr((string)$ba['version'],6);
			while(substr((string)$vrs,-2) == '.0')
				$vrs = substr((string)$vrs,0,-2);
			$GLOBALS['fxptdata']['lvalues'][$mc][$ln]['version_nr']='~<span style="color:#'.$col.';">'.$vrs.'</span>';
			// Date
			$GLOBALS['fxptdata']['lvalues'][$mc][$ln]['datum']='~<span style="color:#'.$col.';">'.get_entry($ba['datetime'],'date').' '.get_entry($ba['datetime'].'00','time').'</span>';
			// Number
			$tt='';
			$tj='';
			if(strlen((string)$ba['info']['tables']))
			{
				$tl=1;
				$ca=0;
				$tt  = '<b style=\'white-space:nowrap;\'>'.$tha[2].'</b><br /><br />';	// Listing of History tables
				$tt .= '<table border=0 cellpadding=1 cellspacing=2 style=\'min-width:400px;\'>';
				$tt .= '<tr><td class=crhd align=center><b>#</b></td><th class=crhd align=left>&nbsp;<b>'.$lit_table.'</b>&nbsp;</th><th class=crhd align=right>&nbsp;<b>'.$lit_number.'</b>&nbsp;</th></tr>';	// Table | Number
				$ite=explode('|', $ba['info']['tables']);
				foreach($ite as $itt)
				{
					$ta=explode(',', $itt);
					if(sizeof($ta) > 1)
					{
						$tts='none';
						if($tl < 12)
							 $tts='table-row';
						else if($tl == 12)
						{
							$tt .= '<tr note style=display:table-row><td colspan=3 align=center bgcolor=#ffffff class=red><br />'.str_replace('$var1', sizeof($ite)-$tl, $tm).'<br /><br /><br /></td></tr>';
							$tj='#';
						}
						$tt .= '<tr style=display:'.$tts.'><td align=right>&nbsp;'.$tl.'.&nbsp;</td><td>&nbsp;'.$ta[0].'&nbsp;</td><td align=right>&nbsp;'.get_entry($ta[1],'integer_sp').'&nbsp;</td></tr>';

						$ca += (int)$ta[1];
						$tl++;
					}
				}
				$tt .= '<tr><td colspan=3 align=right style=\'border-top:1px solid #888;\'>&nbsp;<b>'.get_entry($ca,'integer_sp').'</b>&nbsp;</td></tr>';
				$tt .= '</table>';
			}
			$GLOBALS['fxptdata']['lvalues'][$mc][$ln]['anz']='~#'.$tj.$tt.'|~<span style="color:#'.$col.';">'.get_entry($ba['amount'],'integer_sp').'</span><img src="'.$GLOBALS['gfxpath'].'tt.png" style="margin-left:4px;padding-bottom:2px;">';
			// Size
			$GLOBALS['fxptdata']['lvalues'][$mc][$ln]['dateigroesse']='~<span style="color:#'.$col.';">'.get_entry(ceil($ba['filesize']/1024),'integer_sp').' KB</span>';
			// Link: Info/Button
			$tt='<b class=green>'.$bla[6].'</b>';	// Latest valid Backup
			$img='green';
			if(strlen((string)$ba['error']))
			{
				$tt='<b class=red>'.$lit_error.':</b><br /><br /><ul style=padding-left:12px;>'.$ba['error'].'</ul>';	// ERROR
				$img='red';
			}
			else if(!$ba['latest'])
			{
				$tt='<b class=grey>'.$bla[7].'</b>';	// Earlier valid Backup
				$img='grey';
			}
			$GLOBALS['fxptdata']['lvalues'][$mc][$ln]['link']='~#'.$tt.'|~<table><tr><td style="height:27px;"><img src="'.$GLOBALS['gfxpath'].'tl_'.$img.'.png" style="margin:0 10px;"></td>';
			if((!strlen((string)$ba['error']) && ($GLOBALS['fxpglobals']['action'] == FXP_CHANGE)) || ($GLOBALS['fxpglobals']['action'] == FXP_DELETE))
			{
//function fieldSubmit($fieldname, $value, $type='sub', $width=0, $dis=false, $nva=false, $force=false, $ltr='', $gparam='', $jsfunc='', $style='', $form='')
				if($GLOBALS['fxpglobals']['action'] == FXP_CHANGE)
					$btxt=$bla[3];	// Restore Year
				else
					$btxt=$bla[5];	// Delete Year
				$btn=fieldSubmit('hm_action_'.$ln, $btxt.': '.$ba['year']);
				$GLOBALS['fxptdata']['lvalues'][$mc][$ln]['link'] .= '<td>'.$btn.'</td>';
			}
			$GLOBALS['fxptdata']['lvalues'][$mc][$ln]['link'] .= '</tr></table>';
			// Hidden
			$GLOBALS['fxptdata']['lvalues'][$mc][$ln]['nv_text1']=$ba['filename'];

			$ln++;
		}

		// Empty line with top border
		$GLOBALS['msk_nbcol_549_'.$ln]=$GLOBALS['msk_nbcol'];
		fxf_maskColumnBorder($fa, 1, -1, $mc, $ln, true);
		$ln++;
	}
}
?>