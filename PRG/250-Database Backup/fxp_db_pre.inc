<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : fxp_db_pre.inc                                               //
// Version     : 24.1                                                         //
// Begin       : 2022-05-11                                                   //
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
 * PF 250: Database Backup
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
$version_nr_nw=true;
$Dateiname_nw=true;
$Datum_nw=true;
$anz_nw=true;
$dateigroesse_nw=true;

// Literals
$bla=explode('|', meldung(3386));	// Purge Year|Purge Earliest Year|Restore Selected Years|Restore Year|Delete Selected Years|Delete Year|Latest valid Backup|Earlier valid Backup|Backup Client|Restore Latest Valid Client Backup|Restore Client Backup|Delete Selected Client Backups|Delete Client Backup
//fxDebug($bla,'$bla', 0);
$lit_table=get_text(1440,'15');		// Table
$lit_number=get_text(1571,'10');	// Number
$lit_total=meldung(1555);			// Grand-total
$lit_error=meldung(2029);			// ERROR

// Tooltip texts
$tha=explode('|', meldung(3396));	// Listing of tables|Listing of Client tables|Listing of History tables
$tm=meldung(3358,false);			// <b>&gt; $var1 additional entries exist &lt;</b><br /><i class=lightgrey>(Click to display the complete list in a new popup)</i>

// Error infos
$bea=explode('|', meldung(3389));	// Info missing|Header entry missing|Wrong header client|Wrong header year|Wrong header date|Wrong header version|Wrong header checksum|Wrong info size|Wrong info year|Wrong info client|Wrong info date|Wrong info version|Wrong info database name|Wrong info database type|Wrong info checksum|Old backup version file
//fxDebug($bea,'$bea', 0);

// Display reload after successful restore
if(strlen((string)$successreload))
{
	$hsa=explode('|', meldung(3394)); // Success List|The data of the $var1file$var2 was successfully restored into the database|The $var1file$var2 was successfully deleted from the [<b>DMS</b>]|The $var1file$var2 was successfully deleted in the client folder$var3|To avoid problems due to the newly imported data, you should now log out of fx-project and log in again
//fxDebug($hsa,'$hsa', 0);

	// To avoid problems due to the newly imported data, you should now log out of fx-project and log in again
	$maske497_nv=true;
	fxp_warn($successreload.'<div style="margin-top:12px;padding-top:12px;border-top:1px dotted #009f6b;">&rarr; '.$hsa[4].'.</div>','','009f6b');

	// Logout
//function fieldSubmit($fieldname, $value, $type='sub', $width=0, $dis=false, $nva=false, $force=false, $ltr='', $gparam='', $jsfunc='', $style='', $form='')
	$btn=fieldSubmit('logout', 'Logout', 'sub', 200, false, false, false, '', '', '', 'font-size:24px;height:54px;border-radius:24px;', true);
	echo('<table width=100% height=75%><tr><td height="75%" align="center">'.$btn.'</td></tr></table>');
}
// Mode: Create
else if($GLOBALS['fxpglobals']['action'] == FXP_CREATE)
{
	// Get latest backup for this version
	$ima=explode('|', meldung(3395));	// Your last existing backup for the current fx-project version$var1 is from$var2|There is $var2NO$var3 backup for the current fx-project version$var1 yet!
//fxDebug($ima,'$ima', 0);
	$lvcd='';
	$clen=strlen((string)$GLOBALS['fxpglobals']['client']);
	$srch='/_fxp_mbackup'.$GLOBALS['fxpglobals']['client'].'_[0-9]{12}_'.$GLOBALS['fxpglobals']['dbparam']['versionscode_db'].'.*\.txt$/';	// _fxp_mbackup{CLIENT}_{Datetime: YYYYMMDDHHII}_{Version: VVVVVVVVV}*.[txt|sql]
	$sa=fxScanDir($cfa['cdmspath'], false, true, $srch);
//fxDebug($sa,'$sa: '.$cfa['cdmspath'].' with search pattern '.$srch, 0);
	if($sa['file_entries'] > 0)
	{
		foreach($sa['files'] as $filename => $filepath)
		{
			$cvcd=substr((string)$filename,13+$clen,12).'00';
			if(!strlen((string)$lvcd) || ($cvcd >= $lvcd))
				$lvcd=$cvcd;
		}
	}

	// Display info
	// ...Your last existing backup for the current fx-project version$var1 is from$var2
	if(strlen((string)$lvcd))
	{
		$icol='009f6b';
		$imsg=strtr($ima[0], array('$var1'=>' '.$GLOBALS['fxpglobals']['dbparam']['version_db'], '$var2'=>':<br /><br /><span class="s5b">'.get_entry($lvcd,'datetime').'</span>'));
	}
	// ...There is $var2NO$var3 backup for the current fx-project version$var1 yet!
	else
	{
		$icol='e1001a';
		$imsg=strtr($ima[1], array('$var1'=>' '.$GLOBALS['fxpglobals']['dbparam']['version_db'], '$var2'=>'<span class="s5b">', '$var3'=>'</span>'));
	}
	fxp_warn($imsg,'',$icol);

	// Get all client tables
	$cta=db_ctables();
//fxDebug($cta,'$cta', 0);

	// Get amount of entries from all client tables
	$bca=array('tables'=>array(), 'amount'=>0);
	foreach($cta as $tn => $dummy)
	{
		$sql="SELECT COUNT(*) as ce FROM ".$tn." WHERE mandanten_id=".$GLOBALS['fxpglobals']['client'];
		$ce=(int)db_value($sql);
//fxDebug($ce,$sql, 0);
		if($ce)
		{
			$bca['tables'][$tn]=$ce;
			$bca['amount'] += $ce;
		}
	}
//fxDebug($bca,'$bca', 0);

	// List tables
	if(sizeof($bca['tables']))
	{
		ksort($bca['tables']);
		$GLOBALS['fxptdata']['lvalues'][$mc]=array();
		$ln=0;

		// Hide columns
		$Checkbox_nv=true;
		$Dateiname_nv=true;
		$version_nr_nv=true;
		$Datum_nv=true;
		$dateigroesse_nv=true;
		$link_nv=true;

		// Display class for Grand-total
		$ta_class='^class_cr3" nowrap style="height:22px;border-top:1px solid #'.$GLOBALS['msk_nbcol'].';border-right:1px solid #'.$GLOBALS['msk_tbcol'].';|~&nbsp;';

		// Mask fields
		$fa=fxf_getMaskFields(497,false,array('Checkbox'=>0, 'Dateiname'=>0, 'version_nr'=>0, 'Datum'=>0, 'dateigroesse'=>0, 'link'=>0));
//fxDebug($fa,'$fa', 0);

		// No [Delete] button
		$Button_Loeschen_nv=true;

		// Rename [Save] button to: Backup Client
		$Button_Speichern_btxt=$bla[8];

		foreach($bca['tables'] as $tablename => $amount)
		{
			// Table names
			$GLOBALS['fxptdata']['lvalues'][$mc][$ln]['tabelle']='~'.$tablename;
			$GLOBALS['fxptdata']['lvalues'][$mc][$ln]['anz']='~'.get_entry($amount,'integer_sp');
			// Hidden
			$GLOBALS['fxptdata']['lvalues'][$mc][$ln]['nv_text1']=$tablename.'|'.$amount;
			$ln++;
		}

		// Grand-total
		fxf_maskColumnBorder($fa, 3, -1, $mc, $ln, true);
		$GLOBALS['fxptdata']['lvalues'][$mc][$ln]['tabelle']=$ta_class.'<b>'.$lit_total.'</b>';
		$GLOBALS['fxptdata']['lvalues'][$mc][$ln]['anz']=$ta_class.'<b>'.get_entry($bca['amount'],'integer_sp').'</b>&nbsp;';
		$ln++;

		// Empty line with top border
		$GLOBALS['msk_nbcol_549_'.$ln]=$GLOBALS['msk_sbcol'];
		fxf_maskColumnBorder($fa, 1, -1, $mc, $ln, true);
		$ln++;
	}
}
// Mode: Display, Change or Delete
else
{
	$cba=array();

	// Search for existing backups
	if(file_exists($cfa['cdmspath']) && is_dir($cfa['cdmspath']))
	{
		$clen=strlen((string)$GLOBALS['fxpglobals']['client']);
		$srch='/_fxp_mbackup'.$GLOBALS['fxpglobals']['client'].'_[0-9]{12}_[0-9]{9}.*\.(txt|sql)$/';	// _fxp_mbackup{CLIENT}_{Datetime: YYYYMMDDHHII}_{Version: VVVVVVVVV}*.[txt|sql]
		$sa=fxScanDir($cfa['cdmspath'], false, true, $srch);
//fxDebug($sa,'$sa: '.$cfa['cdmspath'].' with search pattern '.$srch, 0);
		if($sa['file_entries'] > 0)
		{
			foreach($sa['files'] as $filename => $filepath)
			{
				$hbd=substr((string)$filename,13+$clen,12);
				$hbv=substr((string)$filename,26+$clen,9);
				$hfs=filesize($filepath);
				$cba[$filename]=array(
					'filepath'=>$filepath, 'filename'=>$filename, 'format'=>'new', 'datetime'=>$hbd, 'version'=>$hbv, 'filesize'=>$hfs, 'amount'=>0,
					'latest'=>false, 'error'=>'',
					'headers'=>array('filename'=>'', 'versioncode'=>'', 'date'=>'', 'checksum'=>''),
					'info'=>array('client'=>'', 'tables'=>'')
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
								if(substr((string)$hln,0,22) == '/'.'*'.' fx-project: BACKUP ')	// Old backup file
								{
//echo('&rarr; Line '.$hal.': <b>Format</b> [OLD]<br />');
									$cba[$filename]['format']='old';
									break;
								}
								else if(substr((string)$hln,0,4+$clen) == '*C|'.$GLOBALS['fxpglobals']['client'].'|')	// Backup info: Client
								{
//echo('&rarr; Line '.$hal.': <b>Add INFO</b> [client]<br />');
									$cba[$filename]['info']['client']=substr((string)$hln,3);
								}
								else if(substr((string)$hln,0,3) == '*C|')		// Backup info: Tables?
								{
//echo('&rarr; Line '.$hal.': <b>Add INFO</b> [tables]<br />');
									$cba[$filename]['info']['tables']=substr((string)$hln,3);
								}
								else
								{
									$hcp=strpos($hln,':');
									if($hcp)
									{
										$hlt=strtolower((string)str_replace(' ', '', trim(substr((string)$hln,3,$hcp-3))));
										if(isset($cba[$filename]['headers'][$hlt]))
										{
//echo('&rarr; Line '.$hal.': <b>Add HEADER</b> ['.$hlt.']<br />');
											$cba[$filename]['headers'][$hlt]=trim(substr((string)$hln,$hcp+1, -3));
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

					if(($cba[$filename]['format'] == 'old') || (strlen((string)$cba[$filename]['info']['client']) && strlen((string)$cba[$filename]['info']['tables'])))
						break;

					$hct++;
				}
//echo('<hr />');

				// Old format?
				if($cba[$filename]['format'] == 'old')
				{
					$cba[$filename]['error'] .= '<li>'.$bea[15].'</li>';	// Old backup version file
				}
				// New format
				else
				{
					// Do we have all necessary entries?
					if(!strlen((string)$cba[$filename]['info']['client']) || !strlen((string)$cba[$filename]['info']['tables']))
						$cba[$filename]['error'] .= '<li>'.$bea[0].'</li>';	// Info missing
					if(!strlen((string)$cba[$filename]['headers']['filename']) || !strlen((string)$cba[$filename]['headers']['versioncode']) || !strlen((string)$cba[$filename]['headers']['date']) || !strlen((string)$cba[$filename]['headers']['checksum']))
						$cba[$filename]['error'] .= '<li>'.$bea[1].'</li>';	// Header entry missing
					if(!strlen((string)$cba[$filename]['error']))
					{
						$hfe=explode('-', substr((string)$cba[$filename]['headers']['filename'],0,-4));
//fxDebug($hfe,'$hfe', 0);
						// Wrong header client?
						$hfc=-1;
						if(sizeof($hfe) > 1)
							$hfc=(int)$hfe[1];
						if($hfc != $GLOBALS['fxpglobals']['client'])
							$cba[$filename]['error'] .= '<li>'.$bea[2].'</li>';
						// Wrong header date?
						if(substr((string)$cba[$filename]['datetime'],0,8) != str_replace('-', '', $cba[$filename]['headers']['date']))
							$cba[$filename]['error'] .= '<li>'.$bea[4].'</li>';
						// Wrong header version?
						if($cba[$filename]['version'] != $cba[$filename]['headers']['versioncode'])
							$cba[$filename]['error'] .= '<li>'.$bea[5].'</li>';
						// Wrong header checksum?
						$chk=lineCode($cba[$filename]['headers']['filename'],'CLIENTBACKUP');
						if($chk != $cba[$filename]['headers']['checksum'])
							$cba[$filename]['error'] .= '<li>'.$bea[6].'</li>';

						$hia=explode('|', $cba[$filename]['info']['client']);
//fxDebug($hia,'$hia: '. $cba[$filename]['info']['client'], 0);
						if(sizeof($hia) < 8)
							$cba[$filename]['error'] .= '<li>'.$bea[7].'</li>';	// Wrong info size
						else
						{
							$cba[$filename]['amount']=(int)$hia[1];

							// Wrong info client?
							if((int)$hia[0] != $GLOBALS['fxpglobals']['client'])
								$cba[$filename]['error'] .= '<li>'.$bea[9].'</li>';
							// Wrong info date?
							if(substr((string)$cba[$filename]['datetime'],0,8) != substr((string)$hia[2],0,8))
								$cba[$filename]['error'] .= '<li>'.$bea[10].'</li>';
							// Wrong info version?
							if($cba[$filename]['version'] != $hia[3])
								$cba[$filename]['error'] .= '<li>'.$bea[11].'</li>';
							// Wrong info database name?
							if($hia[5] != $GLOBALS['fxpglobals']['dbparam']['dbname'])
								$cba[$filename]['error'] .= '<li>'.$bea[12].'</li>';
							// Wrong info database type?
							if((int)$hia[6] != $GLOBALS['fxpglobals']['dbparam']['dbtype'])
								$cba[$filename]['error'] .= '<li>'.$bea[13].'</li>';
							// Wrong info checksum?
							$chk=lineCode($hia[0].'|'.$hia[1].'|'.$hia[2].'|'.$hia[3].'|'.$hia[4].'|'.$hia[5].'|'.$hia[6], 'CI-'.$hia[0].'-'.$hia[3]);
							if($chk != $hia[7])
								$cba[$filename]['error'] .= '<li>'.$bea[14].'</li>';
						}
					}
				}
			}
		}
	}

	// Mark latest valid version entry
	$lvcb='';
	if(sizeof($cba))
	{
		foreach($cba as $filename => $ba)
		{
			if(!strlen((string)$ba['error']))
			{
				if($ba['version'] == $GLOBALS['fxpglobals']['dbparam']['versionscode_db'])
					$lvcb=$filename;
				else
					$cba[$filename]['error']='<li>'.$bea[15].'</li>';	// Old backup version file
			}
		}
		if(strlen((string)$lvcb))
			$cba[$lvcb]['latest']=true;
	}
//fxDebug($cba,'$cba: '.$cfa['cdmspath'], 0);
//fxDebug($lvcb,'$lvcb', 0);

	// Display existing backups
	if(sizeof($cba))
	{
		ksort($cba, SORT_STRING);

		$maske549_trheight=32;
		$GLOBALS['fxptdata']['lvalues'][$mc]=array();
		$ln=0;

		// Hide columns
		if($GLOBALS['fxpglobals']['action'] != FXP_DELETE)
			$Checkbox_nv=true;
		$Tabelle_nv=true;

		// Mask fields
		$fa=fxf_getMaskFields(497,false,array('Checkbox'=>0, 'Tabelle'=>0, 'link'=>0));
//fxDebug($fa,'$fa', 0);

		// Display [Save] button only in Change mode and rename it to: 	Restore Latest Valid Client Backup
		if(($GLOBALS['fxpglobals']['action'] != FXP_CHANGE) || !strlen((string)$lvcb))
			$Button_Speichern_nv=true;
		else
			$Button_Speichern_btxt=$bla[9];

		// Display [Delete] button only in Delete mode and rename it to: Delete Selected Client Backups
		if($GLOBALS['fxpglobals']['action'] != FXP_DELETE)
			$Button_Loeschen_nv=true;
		else
			$Button_Loeschen_btxt=$bla[11];

		foreach($cba as $filename => $ba)
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
			// Version
			$vrs=(int)substr((string)$ba['version'],0,3).'.'.(int)substr((string)$ba['version'],3,3).'.'.(int)substr((string)$ba['version'],6);
			while(substr((string)$vrs,-2) == '.0')
				$vrs = substr((string)$vrs,0,-2);
			$GLOBALS['fxptdata']['lvalues'][$mc][$ln]['version_nr']='~<b style="color:#'.$col.';">'.$vrs.'</b>';
			// File Name
			$GLOBALS['fxptdata']['lvalues'][$mc][$ln]['dateiname']='~<span style="color:#'.$col.';">'.$ba['filename'].'</span>';
			// Date
			$GLOBALS['fxptdata']['lvalues'][$mc][$ln]['datum']='~<span style="color:#'.$col.';">'.get_entry($ba['datetime'],'date').' '.get_entry($ba['datetime'].'00','time').'</span>';
			// Number
			if($ba['format'] == 'old')
				$GLOBALS['fxptdata']['lvalues'][$mc][$ln]['anz']='~<span style="color:#'.$col.';">?</span>';
			else
			{
				$tt='';
				$tj='';
				if(strlen((string)$ba['info']['tables']))
				{
					$tl=1;
					$ca=0;
					$tt  = '<b style=\'white-space:nowrap;\'>'.$tha[1].'</b><br /><br />';	// Listing of Client tables
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
			}
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
			$valid=false;
			if((!strlen((string)$ba['error']) && ($GLOBALS['fxpglobals']['action'] == FXP_CHANGE)) || ($GLOBALS['fxpglobals']['action'] == FXP_DELETE))
			{
//function fieldSubmit($fieldname, $value, $type='sub', $width=0, $dis=false, $nva=false, $force=false, $ltr='', $gparam='', $jsfunc='', $style='', $form='')
				$valid=true;
				if($GLOBALS['fxpglobals']['action'] == FXP_CHANGE)
					$btxt=$bla[10];	// Restore Client Backup
				else
					$btxt=$bla[12];	// Delete Client Backup
				$btn=fieldSubmit('hm_action_'.$ln, $btxt);
				$GLOBALS['fxptdata']['lvalues'][$mc][$ln]['link'] .= '<td>'.$btn.'</td>';
			}
			$GLOBALS['fxptdata']['lvalues'][$mc][$ln]['link'] .= '</tr></table>';
			// Hidden
			if($valid)
				$GLOBALS['fxptdata']['lvalues'][$mc][$ln]['nv_text1']=$ba['filename'];

			$ln++;
		}

		// Empty line with top border
		$GLOBALS['msk_nbcol_549_'.$ln]=$GLOBALS['msk_nbcol'];
		fxf_maskColumnBorder($fa, 1, -1, $mc, $ln, true);
		// ...Hidden: Latest valid backup
		$GLOBALS['fxptdata']['lvalues'][$mc][$ln]['nv_text1']='*'.$lvcb;
		$ln++;
	}
}
?>