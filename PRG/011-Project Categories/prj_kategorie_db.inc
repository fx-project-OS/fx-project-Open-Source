<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : prj_kategorie_db.inc                                         //
// Version     : 24.1                                                         //
// Begin       : 2020-10-14                                                   //
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
 * PF 11: Project Categories - Database Actions
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 24.1
 */

$prjcat_debug=false;

if($prjcat_debug) fxDebug('_POST');

if(fxIsArray($_POST) && isset($_POST['Button_Speichern']))
{
	// Alle Sprachen ermitteln
	$lng=array();
	$la=fxGetAllLanguages();
	foreach($la as $lid => $a)
	{
		$lng[$lid]=false;
		if($lid == $GLOBALS['fxpglobals']['lang'])
			$lng[$lid]=true;
	}
if($prjcat_debug) fxDebug($lng, '$lng', 0);

	// Projektkategoriegruppe
	$GLOBALS['sgi']=array();
	$GLOBALS['sga']=array();
	$GLOBALS['sg'] =array();
	$cnt=0;
	foreach($_POST as $pk => $pv)
	{
		$cnt++;
		if(substr((string)$pk, 0, 2) == 'gi')
		{
			$gc=(int)substr((string)$pk, 2);

			$gi=(int)$_POST['gi'.$gc];
			$gm=(int)$_POST['gm'.$gc];

			if(isset($_POST['gp'.$gc]))
			{
				$gp=(int)$_POST['gp'.$gc];
				$gg=trim((string)$_POST['gg'.$gc]);
				$gv=trim((string)$_POST['gv'.$gc]);
				$chg=true;
			}
			else
			{
				$gp=$GLOBALS['pg'][$GLOBALS['pga'][$gi]]['pos'];
				$gg=$GLOBALS['pg'][$GLOBALS['pga'][$gi]]['col'];
				$gv=$GLOBALS['pg'][$GLOBALS['pga'][$gi]]['val'];
				$chg=false;
			}

			$gid=$gi;
			if($gid < 0)
				$gid='n'.$gc;

			if(!strlen((string)$gg))
				$gg=0;
			else
			{
				$gg=(int)$gg;
				if($gg < 0)
					$gg=0;
			}

			$unique_id=$gp.'_'.str_replace(' ', '', strtolower((string)$gv)).'_'.$gid;

			$GLOBALS['sgi'][$gc]=$gid;
			$GLOBALS['sga'][$gid]=$unique_id;
			$GLOBALS['sg'][$unique_id]=array('aid'=>$gi, 'id'=>$gi, 'mid'=>$gm, 'pos'=>$gp, 'col'=>$gg, 'val'=>$gv, 'chg'=>$chg, 'cat'=>array());
		}
	}
	ksort($GLOBALS['sg']);
if($prjcat_debug) fxDebug($GLOBALS['sgi'], 'sgi');
if($prjcat_debug) fxDebug($GLOBALS['sga'], 'sga');
if($prjcat_debug) fxDebug($GLOBALS['sg'], 'sg');

	// Projektkategorie
	$GLOBALS['sca']=array();
	$GLOBALS['sc'] =array();
	$GLOBALS['scl']=array();
	$cnt=0;
	foreach($_POST as $pk => $pv)
	{
		$cnt++;
		if(substr((string)$pk, 0, 2) == 'ci')
		{
			$cc=substr((string)$pk, 2);
			$gc=explode('_', $cc);

			$gi=(int)$gc[0];
			$gid=$GLOBALS['sgi'][$gi];

			$ci=(int)$_POST['ci'.$cc];
			$cm=(int)$_POST['cm'.$cc];
			$cu=(int)$_POST['cu'.$cc];

			if(isset($_POST['cp'.$cc]))
			{
				$cp=(int)$_POST['cp'.$cc];
				$cg=trim((string)$_POST['cg'.$cc]);
				$cv=trim((string)$_POST['cv'.$cc]);
				$chg=true;
			}
			else
			{
				$cp=$GLOBALS['pc'][$GLOBALS['pca'][$ci]]['pos'];
				$cg=$GLOBALS['pc'][$GLOBALS['pca'][$ci]]['col'];
				$cv=$GLOBALS['pc'][$GLOBALS['pca'][$ci]]['val'];
				$chg=false;
			}

			$cid=$ci;
			if($cid < 0)
				$cid='n'.$cnt;

			if(!strlen((string)$cg))
				$cg=0;
			else
			{
				$cg=(int)$cg;
				if($cg < 0)
					$cg=0;
			}

			$unique_id=$cp.'_'.str_replace(' ', '', strtolower((string)$cv)).'_'.$cid;

			$GLOBALS['sca'][$cid]=$unique_id;
			$GLOBALS['sc'][$unique_id]=array('cnt'=>$cnt, 'aid'=>$ci, 'id'=>$ci, 'mid'=>$cm, 'pos'=>$cp, 'col'=>$cg, 'val'=>$cv, 'prj'=>$cu, 'chg'=>$chg);

			$GLOBALS['scl'][$cid]=$gid;
		}
	}
	ksort($GLOBALS['sc']);
if($prjcat_debug) fxDebug($GLOBALS['sca'], 'sca');
if($prjcat_debug) fxDebug($GLOBALS['sc'], 'sc');
if($prjcat_debug) fxDebug($GLOBALS['scl'], 'scl');

	$dba=array('group'=>array(), 'category'=>array(), 'assign'=>array());

	// Projektkategoriegruppe/Projektkategorie
	$gc=array('group', 'category');
	foreach($gc as $type)
	{
		if($type == 'group')
		{
			$src=$GLOBALS['pga'];
			$dst=$GLOBALS['sga'];
			$org=$GLOBALS['pg'];
			$dat=$GLOBALS['sg'];
			$fid=10;
		}
		else
		{
			$src=$GLOBALS['pca'];
			$dst=$GLOBALS['sca'];
			$org=$GLOBALS['pc'];
			$dat=$GLOBALS['sc'];
			$fid=1;
		}

		// Neuanlagen/Änderungen ermitteln...
		foreach($dst as $id => $unique_id)
		{
if($prjcat_debug) echo($type.': '.$id.' -&gt; '.$unique_id.'<br>');
			$pos=(int)$dat[$unique_id]['pos'];
			$col=trim((string)$dat[$unique_id]['col']);
			$val=trim((string)$dat[$unique_id]['val']);
			if(!strlen((string)$val))
				$val=$val_empty;
			if(!strlen((string)$col))
				$col=0;
			else
			{
				$col=(int)$col;
				if($col < 0)
					$col=0;
			}

			// Neuanlage
			if(substr((string)$id, 0, 1) == 'n')
			{
				$wid=id_max('wertetab', 'wertetabid', '', $GLOBALS['fxpglobals']['client']);
				$bg='';
				$tbw=$val;
				if($col > 0)
				{
					$bg='<bg'.pk_backgrounds(3,$col).'>';
					$tbw=$tbw.$bg;
				}
//echo('<pre>'.fxHtmlEncode($tbw).'</pre>');

				if($type == 'group')
					$GLOBALS['sg'][$unique_id]['aid']=$wid;
				else
					$GLOBALS['sc'][$unique_id]['aid']=$wid;

				foreach($lng as $lid => $slng)
				{
					if(!$slng && (mb_substr((string)$val, 0, 1) != '§'))
						$dbval="'§".convert_string($tbw, 'todb')."'";
					else
						$dbval="'".convert_string($tbw, 'todb')."'";

					$dba[$type][$id.'_'.$lid]=array(
						'pos' => $pos,
						'col' => $col,
						'bg'  => $bg,
						'val' => $val,
						'sql' => "INSERT INTO wertetab (mandanten_id, wertetabid, id_feld, id_sprache, position, tabwert, satzvers, archiv) VALUES (".(INT)$GLOBALS['fxpglobals']['client'].", ".$wid.", ".$fid.", ".$lid.", ".$pos.", ".$dbval.", 0, 0)"
					);
				}
			}
			else
			{
				$ounique_id=$src[$id];
				$opos=(int)$org[$ounique_id]['pos'];
				$ocol=trim((string)$org[$ounique_id]['col']);
				$oval=trim((string)$org[$ounique_id]['val']);
				if(!strlen((string)$ocol))
					$ocol=0;
				else
				{
					$ocol=(int)$ocol;
					if($ocol < 0)
						$ocol=0;
				}
				$bg='';
				$tbw=$val;
				if($col > 0)
				{
					$bg='<bg'.pk_backgrounds(3,$col).'>';
					$tbw=$tbw.$bg;
				}
//echo('<pre>'.fxHtmlEncode($tbw).'</pre>');

				// Änderung
				if(($opos != $pos) || ($ocol != $col) || ($oval != $val))
				{
if($prjcat_debug) echo('- Compare: pos('.$opos.', '.$pos.') - col ('.$ocol.', '.$col.') - val('.$oval.', '.$val.')<br>');
					$wid=$id;
					$dlv=array();

					$sql="SELECT id_sprache, tabwert FROM wertetab WHERE wertetabid=".$wid." AND id_sprache<>".$GLOBALS['fxpglobals']['lang']." AND mandanten_id=".(INT)$GLOBALS['fxpglobals']['client'];
					$tmp=db_values($sql, '*');
					if(fxIsArray($tmp))
					{
						foreach($tmp as $t)
							$dlv[(int)$t['id_sprache']]=pk_backgrounds(1, $t['tabwert']);
					}
if($prjcat_debug) fxDebug($dlv, $sql);

					foreach($lng as $lid => $slng)
					{
						if(!$slng && (mb_substr((string)$val, 0, 1) != '§'))
						{
							if(mb_substr((string)$dlv[$lid]['txt'], 0, 1) == '§')
								$dbval="'§".convert_string($tbw, 'todb')."'";
							else
								$dbval="'".convert_string($bg.$dlv[$lid]['txt'], 'todb')."'";
						}
						else
							$dbval="'".convert_string($tbw, 'todb')."'";

						$dba[$type][$id.'_'.$lid]=array(
							'pos' => $pos,
							'col' => $col,
							'bg'  => $bg,
							'val' => $val,
							'sql' => "UPDATE wertetab SET position=".$pos.", tabwert=".$dbval." WHERE wertetabid=".$wid." AND id_sprache=".$lid." AND mandanten_id=".(INT)$GLOBALS['fxpglobals']['client']
						);
					}
				}
			}
		}
if($prjcat_debug) echo('<hr>');

		// Löschungen ermitteln...
		foreach($src as $id => $ounique_id)
		{
if($prjcat_debug) echo($type.': '.$id.' -&gt; '.$ounique_id.'<br>');
			if(!isset($dst[$id]))
			{
				$wid=$id;

				$pos=(int)$org[$ounique_id]['pos'];
				$val=trim((string)$org[$ounique_id]['val']);

				$dba[$type][$id]=array(
					'pos' => $pos,
					'val' => $val,
					'sql' => "DELETE FROM wertetab WHERE wertetabid=".$wid." AND mandanten_id=".(INT)$GLOBALS['fxpglobals']['client']
				);
			}
		}
if($prjcat_debug) echo('<hr>');
	}

	// Zuordnungen
	$type='assign';
	$src=$GLOBALS['pcl'];
	$dst=$GLOBALS['scl'];
if($prjcat_debug) fxDebug($src, '$src');
if($prjcat_debug) fxDebug($dst, '$dst');

	// Neuanlagen/Änderungen ermitteln...
	foreach($dst as $id => $unique_id)
	{
		$ucid=$GLOBALS['sca'][$id];
		$cid =(int)$GLOBALS['sc'][$ucid]['aid'];
		$cpos=(int)$GLOBALS['sc'][$ucid]['pos'];
		$cval=trim((string)$GLOBALS['sc'][$ucid]['val']);
		if(!strlen((string)$cval))
			$cval=$val_empty;

		$ugid=$GLOBALS['sga'][$unique_id];
		$gid =(int)$GLOBALS['sg'][$ugid]['aid'];
		$gpos=(int)$GLOBALS['sg'][$ugid]['pos'];
		$gval=trim((string)$GLOBALS['sg'][$ugid]['val']);
		if(!strlen((string)$gval))
			$gval=$val_empty;
if($prjcat_debug) echo($type.': '.$id.' -&gt; '.$unique_id.' (cid='.$cid.', cpos='.$cpos.', cval='.$cval.' -&gt; gid='.$gid.', gpos='.$gpos.', gval='.$gval.')<br>');

		// Neuanlage
		if(substr((string)$id, 0, 1) == 'n')
		{
			// nicht zur Gruppe: "Ohne Zuordnung"
			if($gid != 2779)
			{
				$dba[$type][$id]=array(
					'pos' => $cpos,
					'val' => $cval,
					'sql' => "INSERT INTO kategoriezuordnung (mandanten_id, kategoriegruppe_id, kategorie_id) VALUES (".(INT)$GLOBALS['fxpglobals']['client'].", ".$gid.", ".$cid.")"
				);
			}
		}
		else
		{
			if(isset($src[$id]))
			{
				// nicht zur Gruppe: "Ohne Zuordnung"
				if($gid == 2779)
				{
					$dba[$type][$id]=array(
						'pos' => $cpos,
						'val' => $cval,
						'sql' => "DELETE FROM kategoriezuordnung WHERE kategorie_id=".$cid." AND mandanten_id=".(INT)$GLOBALS['fxpglobals']['client']
					);
				}
				else if($gid != $src[$id])
				{
					$dba[$type][$id]=array(
						'pos' => $cpos,
						'val' => $cval,
						'sql' => "UPDATE kategoriezuordnung SET kategoriegruppe_id=".$gid." WHERE kategorie_id=".$cid." AND mandanten_id=".(INT)$GLOBALS['fxpglobals']['client']
					);
				}
			}
			// nicht zur Gruppe: "Ohne Zuordnung"
			else if($gid != 2779)
			{
				$dba[$type][$id]=array(
					'pos' => $cpos,
					'val' => $cval,
					'sql' => "INSERT INTO kategoriezuordnung (mandanten_id, kategoriegruppe_id, kategorie_id) VALUES (".(INT)$GLOBALS['fxpglobals']['client'].", ".$gid.", ".$cid.")"
				);
			}
		}
	}
if($prjcat_debug) echo('<hr>');

	// Löschungen ermitteln...
	if(sizeof($src))
	{
		foreach($src as $id => $ounique_id)
		{
if($prjcat_debug) echo($type.': '.$id.' -&gt; '.$ounique_id.'<br>');
			if(!isset($dst[$id]))
			{
				$dba[$type][$id]=array(
					'pos' => $cpos,
					'val' => $cval,
					'sql' => "DELETE FROM kategoriezuordnung WHERE kategorie_id=".$id." AND mandanten_id=".(INT)$GLOBALS['fxpglobals']['client']
				);
			}
		}
if($prjcat_debug) echo('<hr>');
	}

	// Datenbankcalls ausführen
	if(sizeof($dba['group']) || sizeof($dba['category']) || sizeof($dba['assign']))
	{
		// 0:Projektkategoriegruppe, 1:Projektkategorie + 2:Projektkategorie-Zuordnung
		$type_lit=explode('|', meldung(2784, true));

		$ins =0;
		$upd =0;
		$del =0;

		$gins=0;
		$gupd=0;
		$gdel=0;

		$cins=0;
		$cupd=0;
		$cdel=0;

		$ains=0;
		$aupd=0;
		$adel=0;

if($prjcat_debug) fxDebug($dba, '$dba');
		$type_cnt=0;
		foreach($dba as $type => $darray)
		{
			if(sizeof($darray))
			{
				foreach($darray as $did => $ddat)
				{
					$sql=$ddat['sql'];
					$sql_type=strtolower(substr((string)$sql, 0, 3));
if($prjcat_debug) echo('<pre>'.fxHtmlEncode($sql).'</pre>');
					$err=db_x($sql);
					if($err)
					{
						// 149: Fehler beim Anlegen von $var1!
						// 150: Fehler beim Ändern von $var1!
						// 151: Fehler beim Löschen von $var1!
						if($sql_type == 'ins')
							$err_code=149;
						else if($sql_type == 'upd')
							$err_code=150;
						else
							$err_code=151;
						$err_val='<b>'.$type_lit[$type_cnt].'</b> ['.$ddat['pos'].': &quot;<b><font color=#880011>'.$ddat['val'].'</font></b>&quot;] ';
						error_msg($err_code, false, $err_val);

						break 2;
					}
					else
					{
						${$sql_type}++;
						${substr((string)$type, 0, 1).$sql_type}++;
					}
				}
			}

			$type_cnt++;
		}

		if(!$err)
		{
			if(($gins || $cins) && !$ains && !$upd && !$del)
			{
				// 1518: Alle Datensätze wurden erfolgreich angelegt.
				error_msg(1518, false);
			}
			else if(($gdel || $cdel) && !$adel && !$ins && !$upd)
			{
				// 1520: Alle Datensätze wurden erfolgreich gelöscht.
				error_msg(1520, false);
			}
			else
			{
				// 1519: Alle Datensätze wurden erfolgreich geändert.
				error_msg(1519, false);
			}

			pk_getCategories(false);
		}
if($prjcat_debug) $err=true;
	}
	else
	{
		// 160: Es wurden keine Änderungen durchgeführt.
		error_msg(160, false);
	}

	if($err)
		$err_mode=true;
}
?>