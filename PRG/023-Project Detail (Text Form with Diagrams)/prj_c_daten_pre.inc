<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : prj_c_daten_pre.inc                                          //
// Version     : 21.2                                                         //
// Begin       : 2020-10-29                                                   //
// Last Change : 2020-11-16                                                   //
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
 * PF 23: Project Detail (Text Form with Diagrams)
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 21.2
 */

enable_dummies();
setForm(true);

// Hinweistexte
$invoice_msg='<font class=red>'.meldung(2901, false).'</font>';			// Das rechnungsrelevante Feld [$var1] kann nicht mehr abgeändert werden, da dieses Projekt oder diese Aufgabe bereits abgerechnet wurde.
$invoice_chg='<br><br><i class=grey>('.meldung(2902, false).')</i>';	// Als Geschäftsleitung können Sie jedoch dieses Feld auf eigene Verantwortung zum Ändern reaktivieren, indem Sie in das Feld klicken.

// Felder ermitteln
$vPORT='';
if($err)
{
	$vMPID=(int)$_POST['Mutterprojekt_ID'];

	$vVN='';
	$vVNP=substr($_POST['Vorgangsnummer'], 0, -1);
	$vVNS=$_POST['Vorgang_naechster'];

	$vPF='';
	$vNP=$_POST['Name_Projekt'];
	$vPA=$_POST['Projektart'];

	$vAG=$_POST['Ansprechpartner'];
	$vPM=$_POST['Projektmanager'];
	$vPL=$_POST['Projektleiter'];
	$vSPL=$_POST['Vertreter'];

	$vMAID=$_POST['MaArt_ID'];
	$vKID=$_POST['Kategorie_ID'];
	$vPST=$_POST['Projekt_Status'];
	$vPRIO=$_POST['Prioritaet'];
	$vBESCH=$_POST['Beschreibung'];
	$vPORT=$_POST['port'];

	if($app)
	{
		$vFREI='';
		if(isset($_POST['Freigabe']) && $_POST['Freigabe'])
			$vFREI='1';
	}

	if(isset($_POST['pprefix']))
		$vPF=$_POST['pprefix'];

	if(!$vPM && is_array($pinfo))
	{
		if($vAPID)
			$vPM=$pinfo[$vAPID]['data']['projektmanager'];
		else if($vEPID)
			$vPM=$pinfo[$vEPID]['data']['projektmanager'];
	}
}
else if(fxIsArray($pinfo))
{
	$vPF='';

	if($vAPID)
	{
		$vMPID=(int)$pinfo[$vAPID]['data']['mutterprojekt_id'];

		$vVN=$pinfo[$vAPID]['data']['vorgangsnummer'];
		if(strlen($pinfo[$vAPID]['data']['vorgangsnummer']) > 5)
			$vVNP=no_null(substr($pinfo[$vAPID]['data']['vorgangsnummer'], 0, -5));
		else
			$vVNP='';
		$vVNS=(int)substr($pinfo[$vAPID]['data']['vorgangsnummer'], -4);

		$vNP=trim($pinfo[$vAPID]['data']['name_projekt']);
		$vPA=$pinfo[$vAPID]['data']['projektart'];

		$vAG=$pinfo[$vAPID]['data']['ansprechpartner'];
		$vPM=$pinfo[$vAPID]['data']['projektmanager'];
		$vPL=$pinfo[$vAPID]['data']['projektleiter'];
		$vSPL=$pinfo[$vAPID]['data']['vertreter'];

		$vMAID=$pinfo[$vAPID]['data']['fae'];
		$vKID=$pinfo[$vAPID]['data']['kategorie_id'];
		$vPST=$pinfo[$vAPID]['data']['projekt_status'];
		$vPRIO=$pinfo[$vAPID]['data']['prioritaet'];
		$vBESCH=$pinfo[$vAPID]['data']['beschreibung'];
		$vPORT=$pinfo[$vAPID]['data']['port'];

		if($app)
		{
			$vFREI='';
			if(strlen($pinfo[$vAPID]['data']['freigabe']) && $pinfo[$vAPID]['data']['freigabe'])
				$vFREI='1';
		}
	}
	else if($vEPID)
	{
		$vMPID=(int)$pinfo[$vEPID]['data']['mutterprojekt_id'];

		$vPA=$vPART;

		$vAG=$pinfo[$vEPID]['data']['ansprechpartner'];
		$vPM=$pinfo[$vEPID]['data']['projektmanager'];
		$vPL=$pinfo[$vEPID]['data']['projektleiter'];
		$vSPL=$pinfo[$vEPID]['data']['vertreter'];

		$vMAID=$pinfo[$vEPID]['data']['fae'];
		$vKID=$pinfo[$vEPID]['data']['kategorie_id'];
		$vPST=$pinfo[$vEPID]['data']['projekt_status'];
		$vPRIO=$pinfo[$vEPID]['data']['prioritaet'];

		$sql="SELECT projekt_id FROM projekte WHERE elter=".$vMPID." AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
		$eltex=(int)db_value($sql);
		// Wird eine Aufgabe direkt unter ein Mutterprojekt gehängt dann zusätzliche Felder vorbelegen
		if(!$eltex && ($pinfo[$vEPID]['data']['projektart'] == FXP_PRJ_MAIN) && ($vPART == FXP_PRJ_TASK))
		{
			$vNP=trim($pinfo[$vEPID]['data']['name_projekt']).'*';
			$vBESCH=$pinfo[$vEPID]['data']['beschreibung'];
		}
	}
}
else
{
	$vMPID=(int)$Projekt_ID;

	$vPST=FXP_PS_PLANNED;
	$vPRIO=446;	// mittel
}
if(!$vPA)
	$vPA=$vPART;

// Vorgangsnummer
if(($GLOBALS['fxpglobals']['action'] == FXP_DISPLAY) || ($GLOBALS['fxpglobals']['action'] == FXP_DELETE))
{
	$Vorgangsnummer_nv=true;
	$maskenfeldwerte['vorgang_naechster']='~'.no_null($vVN);
	$Vorgang_naechster_sf=2;
}
else
{
	if($vPART == FXP_PRJ_MAIN)
		$Vorgangsnummer_nv=true;

	if(!$err && ($GLOBALS['fxpglobals']['action'] == FXP_CREATE))
	{
		if($vPART == FXP_PRJ_MAIN)
		{
			$max_VN=(int)db_value("SELECT MAX(vorgangsnummer) FROM projekte WHERE projektart=".FXP_PRJ_MAIN." AND mandanten_id=".$GLOBALS['fxpglobals']['client']);
			if($max_VN)
				$vVNS=$max_VN+1;
			else if(strtolower(substr($GLOBALS['fxpglobals']['settings']['vorgang0'], 0, 1)) == 'n')
				$vVNS=1;
			else
				$vVNS=0;
		}
		else if($vEPID)
		{
			// Höchste VN der Untervorgänge des Elters ermitteln
			$new_VN=db_value("SELECT MAX(vorgangsnummer) FROM projekte WHERE elter=".$vEPID." AND mandanten_id=".$GLOBALS['fxpglobals']['client']);
			// Hat der Elter bisher noch keine Untervorgänge => VN des Elters ermitteln
			if(!$new_VN)
			{
				// neuer Elter ist Vorgang ohne weitere Untervorgänge
				$new_VN=db_value("SELECT vorgangsnummer FROM projekte WHERE projekt_id=".$vEPID." AND mandanten_id=".$GLOBALS['fxpglobals']['client']);
				$vVNP=no_null($new_VN);
				$vVNS=0;
				if(strtolower(substr($GLOBALS['fxpglobals']['settings']['vorgang0'], 0, 1)) == 'n')
					$vVNS=1;
			}
			// Elter ist "Projekt"/"Teilprojekt" mit weiteren Untervorgängen
			else
			{
				$eltpa=db_value("SELECT projektart FROM projekte WHERE projekt_id=".$vEPID." AND mandanten_id=".$GLOBALS['fxpglobals']['client']);
				// Elter ist Mutterprojekt
				if($eltpa == FXP_PRJ_MAIN)
				{
					$vVNP=no_null(substr($new_VN, 0, 4));
					$vVNS=substr($new_VN, 5, 4) + 1;
				}
				else
				{
					$vVNP=no_null(substr($new_VN, 0, -5));
					$vVNS=substr($new_VN, -4) + 1;
				}
			}
		}
	}

	if($vVNP)
		$maskenfeldwerte['vorgangsnummer']=$vVNP.'.';
	$vVNS=min(9999, (int)$vVNS);
	$maskenfeldwerte['vorgang_naechster']=$vVNS;

	if($invoice_sub)
	{
		$Vorgang_naechster_dis=true;	// Eingabefeld disablen
		$Vorgang_naechster_tt=str_replace('$var1', '<b>'.get_text('Vorgang_naechster', '20').'</b>', $invoice_msg); // Tooltip
		if($GLOBALS['fxpglobals']['persdat']['roles'][2]) // GF darf ändern
		{
			$Vorgang_naechster_tt .= $invoice_chg;
			$Vorgang_naechster_ud=true;
		}
	}
}

//Prefix (pprefix) + Name des Vorgangs (Name_Projekt)
$prefix_wildcard=trim($GLOBALS['fxpglobals']['settings']['vnprefix']);
if(($GLOBALS['fxpglobals']['action'] == FXP_DISPLAY) || ($GLOBALS['fxpglobals']['action'] == FXP_DELETE))
{
	if(strlen($prefix_wildcard))
	{
		$vPF=get_prefix_project($vAPID);
		if(strlen($vPF))
		{
			if(substr($vNP,0,strlen($vPF)) === $vPF)
				$vNP=trim(substr($vNP,strlen($vPF)+1));
			else
				$vPF='';
		}
	}
	if(strlen($vPF))
	{
		$Name_Projekt_lit=get_text(1900,'10').':<div style="height:4px;"></div>'.get_text(46,'20');
		$maskenfeldwerte['name_projekt']='~'.$vPF.'<div style="height:4px;"></div>'.$vNP;
	}
	else
		$maskenfeldwerte['name_projekt']='~'.$vNP;
}
else
{
	$vKD=0;
	if(!$err && ($GLOBALS['fxpglobals']['action'] == FXP_CREATE))
	{
		if($vPA == FXP_PRJ_MAIN)
		{
			// Ansprechpartner vorbelegen, der am häufigsten verwendet wurde
			$sql="SELECT COUNT(ansprechpartner) AS anz, ansprechpartner FROM projekte WHERE projektart=".FXP_PRJ_MAIN." AND mandanten_id=".$GLOBALS['fxpglobals']['client']." GROUP BY ansprechpartner ORDER BY anz DESC";
			$tmp=db_values($sql);
//fxDebug($tmp, $sql);
			if(fxIsArray($tmp))
				$vAG=(int)$tmp['ansprechpartner'];
		}
		$vKD=(int)db_value("SELECT elter FROM personen WHERE personen_id=".(int)$vAG." AND mandanten_id=".$GLOBALS['fxpglobals']['client']);
		$vNP='*'.meldung(564);
	}

	if($invoice_act)
	{
		$Name_Projekt_dis=true; // Eingabefeld disablen
		$Name_Projekt_tt=str_replace('$var1', '<b>'.get_text('Name_Projekt', '20').'</b>', $invoice_msg); // Tooltip
		if($GLOBALS['fxpglobals']['persdat']['roles'][2]) // GF darf ändern
		{
			$Name_Projekt_tt .= $invoice_chg;
			$Name_Projekt_ud=true;
		}
		if(strlen($prefix_wildcard) && ($vPA == FXP_PRJ_MAIN))
		{
			$pprefix_dis=true; // Eingabefeld disablen
			$pprefix_tt=str_replace('$var1', '<b>'.get_text(1900, '10').'</b>', $invoice_msg); // Tooltip
			if($GLOBALS['fxpglobals']['persdat']['roles'][2]) // GF darf ändern
			{
				$pprefix_tt .= $invoice_chg;
				$pprefix_ud=true;
			}
		}
	}

	// Prefix
	if(strlen($prefix_wildcard))
	{
		if(!$err)
		{
			if($GLOBALS['fxpglobals']['action'] == FXP_CREATE)
			{
				if($vPA == FXP_PRJ_MAIN)
					$vPF=get_prefix($GLOBALS['fxpglobals']['client'], $vKD);
				else
					$vPF=get_prefix_project($vEPID);
			}
			else
			{
				$vPF=get_prefix_project($vAPID);
				if(strlen($vPF))
				{
					if(substr($vNP,0,strlen($vPF)) === $vPF)
						$vNP=trim(substr($vNP,strlen($vPF)+1));
					else
						$vPF='';
				}
			}
		}
		$dis=false;
		if($vPA != FXP_PRJ_MAIN)
			$dis=true;

		// Feldlängen ermitteln
		$sql="SELECT anzeigegroesse2 FROM maskenfelder WHERE nummer=31 AND eingabefeld=349 AND anzeigeart=8";
		$fwp=abs(db_value($sql));
		$sql="SELECT anzeigegroesse1 FROM maskenfelder WHERE nummer=31 AND eingabefeld=46 AND anzeigeart=6";
		$fwn=abs(db_value($sql));

//function fieldText($fieldname, $value, $width=0, $len=0, $align='left', $mand=false, $dis=false, $nva=false, $tooltip='', $jsfunc='', $ftype='text', $form='', $style_add='', $class_add='')
		$field_pprefix=fieldText('pprefix',$vPF,$fwp,64,'left',false,$dis);
		$field_Name_Projekt=fieldText('Name_Projekt',$vNP,$fwn,255,'left',true);

		$Name_Projekt_lit=get_text(1900,'10').':<div style="height:8px;"></div>'.get_text(46,'20');
		$maskenfeldwerte['name_projekt']='~'.$field_pprefix.'<div style="height:2px;"></div>'.$field_Name_Projekt;
	}
	else
		$maskenfeldwerte['name_projekt']=$vNP;
}

// PCA-Code (pca_code)
$vPCA='';
if(($GLOBALS['fxpglobals']['action'] != FXP_CREATE) && is_array($pinfo) && $vAPID)
	$vPCA=trim($pinfo[$vAPID]['data']['pca']);
if(strlen($vPCA))
	$maskenfeldwerte['pca_code']='~'.$vPCA;
else
	$pca_code_nv=true;

// Art des Vorgangs (Projektart)
$Projektart_nv=false;
if($GLOBALS['fxpglobals']['action'] == FXP_CREATE)
	$Projektart_sf=2;
if(!$Projektart_nv)
	$maskenfeldwerte['projektart']=$vPA;

// Zugeordnet zu Objekt (Elter)
$Elter_nv=false;
if($GLOBALS['fxpglobals']['action'] == FXP_CREATE)
{
	if($vPA == FXP_PRJ_MAIN)
		$Elter_nv=true;
	else
		$Elter_sf=2;
}
if(!$Elter_nv)
{
	if(!$vEPID && (($GLOBALS['fxpglobals']['action'] == FXP_DISPLAY) || ($GLOBALS['fxpglobals']['action'] == FXP_DELETE)))
		$maskenfeldwerte['elter']='~&nbsp;';
	else if(!$Elter_nv)
		$maskenfeldwerte['elter']=$vEPID;
}

// abhängig von Vorgang (Abhaengigkeit)
$Abhaengigkeit_nv=false;
if($GLOBALS['fxpglobals']['action'] == FXP_CREATE)
	$Abhaengigkeit_nv=true;
else
{
	$Abhaengigkeit_lit=get_text(381,'text');
	$vABH='';
	if($vAPID)
	{
		$sql="SELECT projekt_a, projekt_b, abh_typ, tagesanzahl FROM abhaengigkeiten WHERE (projekt_a=".$vAPID." OR projekt_b=".$vAPID.") AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
		$abharr=db_values($sql,'*');
//fxDebug($abharr,$sql);
		if(fxIsArray($abharr))
		{
			$dtypes=getDTypes();
			$apids='';
			foreach($abharr as $abhnr => $abhdat)
				$apids .= ','.$abhdat['projekt_a'].','.$abhdat['projekt_b'];
			$sql="SELECT projekt_id,vorgangsnummer,name_projekt FROM projekte WHERE projekt_id IN (".substr($apids,1).") AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
			$tmp=db_values($sql,'*');
			$aparr=array();
			if(fxIsArray($tmp))
			{
				foreach($tmp as $t)
				{
					if($t['projekt_id'] == $vAPID)
						$pcol='blue';
					else
						$pcol='black';
					$aparr[$t['projekt_id']]='<font class="'.$pcol.'">'.no_null($t['vorgangsnummer']).'</font>&nbsp;-&nbsp;<font class="'.$pcol.'">'.$t['name_projekt'].'</font>';
				}
			}
			foreach($abharr as $abhnr => $abhdat)
			{
				if($abhnr)
					$vABH .= '<br>';
				$vABH .= $aparr[$abhdat['projekt_a']].'&nbsp;<font class="grey">-&gt;</font>&nbsp;'.$dtypes[$GLOBALS['fxpglobals']['lang']][$abhdat['abh_typ']].'&nbsp;<font class="grey">-&gt;</font>&nbsp;'.$aparr[$abhdat['projekt_b']];
			}
		}
	}
	$maskenfeldwerte['abhaengigkeit']='~'.$vABH;
}

// Auftraggeber (Kunde) (Ansprechpartner)
if(($GLOBALS['fxpglobals']['action'] == FXP_DISPLAY) || ($GLOBALS['fxpglobals']['action'] == FXP_DELETE))
	$maskenfeldwerte['ansprechpartner']='~'.person_pic($vAG);
else
{
	$maskenfeldwerte['ansprechpartner']=(int)$vAG;

	if($invoice_act)
	{
		$Ansprechpartner_dis=true; // Auswahlfeld disablen
		$Ansprechpartner_tt=str_replace('$var1', '<b>'.get_text('Ansprechpartner', '20').'</b>', $invoice_msg); // Tooltip
		if($GLOBALS['fxpglobals']['persdat']['roles'][2]) // GF darf ändern
		{
			$Ansprechpartner_tt .= $invoice_chg;
			$Ansprechpartner_ud=true;
		}
	}
}

// Projektmanager (Projektmanager)
$Projektmanager_muss=false;
if(strtolower(substr($GLOBALS['fxpglobals']['settings']['pm_mussfeld'], 0, 1)) == 'j')
{
	$Projektmanager_muss	= true;
	$Projektmanager_ds		= 0;
}
if(($vPA == FXP_PRJ_TASK) || ($GLOBALS['fxpglobals']['action'] == FXP_DISPLAY) || ($GLOBALS['fxpglobals']['action'] == FXP_DELETE))
{
	$Projektmanager_sf=2;
	$maskenfeldwerte['projektmanager']='~'.person_pic($vPM);
}
else
{
	if(!$vPM && ($vPART == FXP_PRJ_MAIN) && ($GLOBALS['fxpglobals']['action'] == FXP_CREATE))
	{
		$pmsql  = "SELECT projektmanager, COUNT(projekt_id) AS anzahl";
		$pmsql .= " FROM projekte";
		$pmsql .= " WHERE mandanten_id=".$GLOBALS['fxpglobals']['client']." AND projektmanager>0";
		$pmsql .= " GROUP BY projektmanager";
		$pmsql .= " ORDER BY anzahl DESC";
		$pmarr=db_values($pmsql);
		if(fxIsArray($pmarr))
			$vPM=(int)$pmarr['projektmanager'];
		else
		{
			$pmsql  ="SELECT personen_id"; 
			$pmsql .= " FROM ma_art_faehigkeiten";
			$pmsql .= " WHERE mafaehigkeit_id=-2 AND maart_id=1 AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
			$pmsql .= " ORDER BY dauer DESC, zeitstempel";
			$vPM=(int)db_value($pmsql);
		}
	}
	$maskenfeldwerte['projektmanager']=$vPM;
}

// Projektleiter (Projektleiter)
if(($GLOBALS['fxpglobals']['action'] == FXP_DISPLAY) || ($GLOBALS['fxpglobals']['action'] == FXP_DELETE))
	$maskenfeldwerte['projektleiter']='~'.person_pic($vPL);
else
	$maskenfeldwerte['projektleiter']=$vPL;

// stell. Projektleiter (Vertreter)
if(($GLOBALS['fxpglobals']['action'] == FXP_DISPLAY) || ($GLOBALS['fxpglobals']['action'] == FXP_DELETE))
{
	if(!$vSPL)
		$maskenfeldwerte['vertreter']='~&nbsp;';
	else
		$maskenfeldwerte['vertreter']='~'.person_pic($vSPL);
}
else
	$maskenfeldwerte['vertreter']=$vSPL;

// Zul. geändert von (Aenderungs_ID)
if($GLOBALS['fxpglobals']['action'] == FXP_CREATE)
	$maskenfeldwerte['aenderungs_id']='~';
else
	$maskenfeldwerte['aenderungs_id']='~'.person_pic((int)$pinfo[$vAPID]['data']['aenderungs_id']).'&nbsp;&nbsp;<i class="grey">'.get_entry($pinfo[$vAPID]['data']['zeitstempel'], 'datetime').'</i>';

// Budgetgenehmigung (budgetgen)
if($vPA >= FXP_PRJ_TASK)
{
	$maskenfeldwerte['budgetgen']='';
	// - Automatisch
	if(strtoupper(substr($GLOBALS['fxpglobals']['settings']['budgetgen'], 0, 1)) == 'J')
		$maskenfeldwerte['budgetgen']='~<i class="grey">['.meldung(1260, true).']</i>';
	// - Personen
	else
	{
		$bgarr='';
		// Alle GF ermitteln
		$sql="SELECT personen_id FROM ma_art_faehigkeiten WHERE maart_id=1 AND mafaehigkeit_id=2 AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
		$gfids=get_array($sql, 'personen_id');
		if(fxIsArray($gfids))
		{
			foreach($gfids as $bgpid => $dmy)
				$bgarr[$bgpid]=2;
		}

		// Alle BG ermitteln, welche Recht auf das akt. Projekt haben
		$bgids=prj_get_bg($Projekt_ID, $GLOBALS['fxpglobals']['action']);
		if(fxIsArray($bgids))
		{
			foreach($bgids as $bgpid => $dmy)
				$bgarr[$bgpid]=-4;
		}

		// - Personen anzeigen
		if(fxIsArray($bgarr))
		{
			$bgpids=array_to_csv($bgarr);
			if(strlen($bgpids))
			{
				$sql  = "SELECT personen_id, pname, vorname, geschlecht FROM personen WHERE personen_id IN (".$bgpids.") AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
				$sql .= " ORDER BY pname, vorname";
				$pers=db_values($sql, '*');
				if(fxIsArray($pers))
				{
					foreach($pers as $p)
					{
						if(strlen($maskenfeldwerte['budgetgen']))
							$maskenfeldwerte['budgetgen'] .= '<br>';
						else
							$maskenfeldwerte['budgetgen']='~';

						$gend=(int)$p['geschlecht'];
						$name=trim($p['pname']);
						$vnam=trim($p['vorname']);
						if(strlen($vnam))
							$name .= ', '.$vnam;

						if($bgarr[$p['personen_id']] != 2)	// Kein GF
							$name='<font class="darkgrey">'.$name.'</font>';

						$maskenfeldwerte['budgetgen'] .= person_pic($p['personen_id']);
					}
				}
			}
		}
		// - Ohne
		if(!strlen($maskenfeldwerte['budgetgen']))
			$maskenfeldwerte['budgetgen']='~<I>['.meldung(388, true).']</I>';
	}
}
else
	$budgetgen_nv=true;

// Zusatzverantwortliche Personen (Personenauswahl)
// Breite Links: Ansprechp. Auftragg.
$lbl=(int)db_value("SELECT spaltenanzahl FROM maskenfelder WHERE nummer=31 AND eingabefeld=349 AND anzeigeart=4");		// 150
$fbl=(int)db_value("SELECT spaltenanzahl FROM maskenfelder WHERE nummer=31 AND eingabefeld=349 AND anzeigeart=8");		// 320
$sbl=(int)db_value("SELECT anzeigegroesse2 FROM maskenfelder WHERE nummer=31 AND eingabefeld=349 AND anzeigeart=8");	// 314
// Breite Rechts: Projektmanager
$lbr=(int)db_value("SELECT spaltenanzahl FROM maskenfelder WHERE nummer=31 AND eingabefeld=1454 AND anzeigeart=4");		// 150
$fbr=(int)db_value("SELECT spaltenanzahl FROM maskenfelder WHERE nummer=31 AND eingabefeld=1454 AND anzeigeart=8");		// 320
$sbr=(int)db_value("SELECT anzeigegroesse2 FROM maskenfelder WHERE nummer=31 AND eingabefeld=1454 AND anzeigeart=8");	// 314

$sql  = "SELECT mafaehigkeit_id, mafaehigkeit_name FROM ma_faehigkeit";
$sql .= " WHERE maart_id=2 AND mandanten_id=".$GLOBALS['fxpglobals']['client']." AND id_sprache=".$GLOBALS['fxpglobals']['lang'];
$sql .= " ORDER BY mafaehigkeit_name";
$zpa=db_values($sql, '*');
if(fxIsArray($zpa))
{
	$pvm=meldung(594, true);	// Auswahl in neuem Fenster öffnen
	$leer=meldung(794, true);	// -leer-
//fxDebug($zpa, $sql, 0);
	$zps='<table border="0" cellpadding="0" cellspacing="1">'.$nl;
	for($zpc=0; $zpc<sizeof($zpa); $zpc += 2)
	{
		$zps .= '<tr>'.$nl;

		$def=0;
		if($err && (($GLOBALS['fxpglobals']['action'] == FXP_CREATE) || ($GLOBALS['fxpglobals']['action'] == FXP_CHANGE)))
			$def=(int)$_POST['pvr_'.$zpa[$zpc]['mafaehigkeit_id']];

		$zps .= '<td nowrap width="'.$lbl.'"><span class="lit">'.$zpa[$zpc]['mafaehigkeit_name'].':&nbsp;</span></td>'.$nl;
		$zps .= pvr_select($zpa[$zpc], $vAPID, $def, $fbl, $sbl);

		if(is_array($zpa[$zpc+1]))
		{
			$def=0;
			if($err && (($GLOBALS['fxpglobals']['action'] == FXP_CREATE) || ($GLOBALS['fxpglobals']['action'] == FXP_CHANGE)))
				$def=(int)$_POST['pvr_'.$zpa[$zpc+1]['mafaehigkeit_id']];

			$zps .= '<td nowrap width="'.$lbr.'" align="right"><span class="lit">'.$zpa[$zpc+1]['mafaehigkeit_name'].':&nbsp;</span></td>'.$nl;
			$zps .= pvr_select($zpa[$zpc+1], $vAPID, $def, $fbr, $sbr);
		}
		else
			$zps .= '<td colspan="2"></td>'.$nl;
		$zps .= '</tr>'.$nl;
	}
	$zps .= '</table>'.$nl;

	$maskenfeldwerte['personenauswahl']='~'.$zps;
}
else
	$Personenauswahl_nv=true;

// Ausführende Person (Personen_ID / EK / VK)
if($vPA == FXP_PRJ_TASK)
{
	$ignore_personenrechte=false;
	if($GLOBALS['fxpglobals']['persdat']['roles'][-3] || $GLOBALS['fxpglobals']['persdat']['roles'][-1] || $GLOBALS['fxpglobals']['persdat']['roles'][1] || $GLOBALS['fxpglobals']['persdat']['roles'][2])
		$ignore_personenrechte=true;

	if(($GLOBALS['fxpglobals']['action'] == FXP_DISPLAY) || ($GLOBALS['fxpglobals']['action'] == FXP_DELETE) || ($vPST == FXP_PS_INACTIVE) || ($vPST == FXP_PS_COMPLETED) || ($sor > 1))
	{
		$Personen_ID_sf=1;
		$EK_sf=1;
		$VK_sf=1;

		$EK_nu=true;
		$VK_nu=true;

		if($sor)
		{
			$maskenfeldwerte['personen_id']	= '~<table border="0" cellpadding="0" cellspacing="1">';
			$maskenfeldwerte['ek']			= '~<table border="0" cellpadding="0" cellspacing="1">';
			$maskenfeldwerte['vk']			= '~<table border="0" cellpadding="0" cellspacing="1">';

			$std=meldung(484);

			$hrcsv='';
			$pccsv='';
			$pecsv='';
			$ppcsv='';
			$spcsv='';

			$etot=0;

			$dek=0.00;
			$dvk=0.00;

			foreach($res as $r)
				$etot += $r['aufwand_std'];

			foreach($res as $r)
			{
				if(strlen($hrcsv))
					$hrcsv .= ',';
				$hrcsv .= $r['personen_id'];

				if($etot > 0)
					$prz=get_double(min(1.00, max(0.00, $r['aufwand_std']/$etot)));
				else
					$prz=get_double(min(1.00, max(0.00, $r['prozent'])));
				if(strlen($pccsv))
					$pccsv .= ',';
				$pccsv .= $prz;

				if(strlen($pecsv))
					$pecsv .= ',';
				$pecsv .= $r['aufwand_std'];

				if(strlen($ppcsv))
					$ppcsv .= ',';
				$ppcsv .= $r['ek_projekt'];

				if(strlen($spcsv))
					$spcsv .= ',';
				$spcsv .= $r['vk_projekt'];

				$dek += $r['ek_projekt']*$prz;
				$dvk += $r['vk_projekt']*$prz;

				$pname=trim($r['pname']);
				$vname=trim($r['vorname']);
				if(strlen($vname))
					$pname .= ',&nbsp;'.$vname;

				$maskenfeldwerte['personen_id']	.= '<tr><td height="20">'.person_pic($r['personen_id'], 20, $r['geschlecht'], $GLOBALS['fxpglobals']['client'], false).'</td><td height="20" nowrap>&nbsp;'.$pname.'&nbsp;<font class="grey">('.get_entry($prz*100.00,'dezimal').'%)</font></td></tr>';
				$maskenfeldwerte['ek']			.= '<tr><td height="20" align="right" nowrap>'.get_entry($r['ek_projekt'],'dezimal').'&nbsp;<font class="grey">'.$GLOBALS['fxpglobals']['settings']['waehrung'].'/'.$std.'</font></td></tr>';
				$maskenfeldwerte['vk']			.= '<tr><td height="20" align="right" nowrap>'.get_entry($r['vk_projekt'],'dezimal').'&nbsp;<font class="grey">'.$GLOBALS['fxpglobals']['settings']['waehrung'].'/'.$std.'</font></td></tr>';
			}

			$maskenfeldwerte['personen_id']	.= '</table><div id="delta_hr" style="display:none;">'.$hrcsv.'|'.$pccsv.'|'.$pecsv.'|'.$ppcsv.'|'.$spcsv.'</div>';
			$maskenfeldwerte['ek']			.= '<tr><td height="20" align="right" nowrap style="border-top:1px solid #cccccc;">&Delta;&nbsp;'.get_entry($dek,'dezimal').'&nbsp;<font class="grey">'.$GLOBALS['fxpglobals']['settings']['waehrung'].'/'.$std.'</font></td></tr></table><div id="delta_pp" style="display:none;">'.(round($dek*100.00)/100.00).'</div>';
			$maskenfeldwerte['vk']			.= '<tr><td height="20" align="right" nowrap style="border-top:1px solid #cccccc;">&Delta;&nbsp;'.get_entry($dvk,'dezimal').'&nbsp;<font class="grey">'.$GLOBALS['fxpglobals']['settings']['waehrung'].'/'.$std.'</font></td></tr></table><div id="delta_sp" style="display:none;">'.(round($dvk*100.00)/100.00).'</div>';

			if(($GLOBALS['fxpglobals']['action'] == FXP_CHANGE) && ($sor > 1))
			{
				// 1900: Die Ressourcenplanung darf nur geändert werden, wenn höchtens eine Ressource der Aufgabe zugeordnet ist. Es wurden aber bereits $var1 Ressourcen zugeteilt! Bitte benutzen Sie hierfür die ausführliche Ressourcenplanungs-Transaktion.
				$msg=str_replace('$var1', $sor, meldung(1900, true));
				$Personen_ID_lit='<font id="persdummy" class="red" tooltip="'.$msg.'" style="cursor:help;">'.get_text(21,'20').'</font>';
			}
		}
		else
		{
			$maskenfeldwerte['personen_id']	= '~&nbsp;';
			$maskenfeldwerte['ek']			= '~&nbsp;';
			$maskenfeldwerte['vk']			= '~&nbsp;';
		}
	}
	else
	{
		if($err)
		{
			$maskenfeldwerte['personen_id']	= $_POST['Personen_ID'];
			$maskenfeldwerte['ek']			= $_POST['EK'];
			$maskenfeldwerte['vk']			= $_POST['VK'];
		}
		else if($sor == 1)
		{
			$maskenfeldwerte['personen_id']	= (int)$res[0]['personen_id'];
			$maskenfeldwerte['ek']			= get_double($res[0]['ek_projekt']);
			$maskenfeldwerte['vk']			= get_double($res[0]['vk_projekt']);
		}
		else
		{
			$maskenfeldwerte['personen_id']	= 0;
			$maskenfeldwerte['ek']			= '';
			$maskenfeldwerte['vk']			= '';
		}
	}
}
else
{
	$Ress_uebern_nv=true;
	$Personen_ID_nv=true;
	$EK_nv=true;
	$VK_nv=true;
}

// Fähigkeitengruppe (MaArt_ID)
if(!$vMAID && (($GLOBALS['fxpglobals']['action'] == FXP_DISPLAY) || ($GLOBALS['fxpglobals']['action'] == FXP_DELETE)))
	$maskenfeldwerte['maart_id']='~&nbsp;';
else
	$maskenfeldwerte['maart_id']=$vMAID;

// Projekt-Kategorie (Kategorie_ID)
if(!$vKID && (($GLOBALS['fxpglobals']['action'] == FXP_DISPLAY) || ($GLOBALS['fxpglobals']['action'] == FXP_DELETE)))
	$maskenfeldwerte['kategorie_id']='~&nbsp;';
else
	$maskenfeldwerte['kategorie_id']=$vKID;

// Status (Projekt_Status)
$Projekt_Status_array=fxf_getReferenceValues('projekt_status');
$tt='';
if(($GLOBALS['fxpglobals']['action'] == FXP_CHANGE) && fxIsArray($pinfo) && fxIsArray($pinfo[$vAPID]) && fxIsArray($pinfo[$vAPID]['data']))
{
	$pda=$pinfo[$vAPID]['data'];
//fxDebug($pda,'$pda: $vAPID='.$vAPID);

	// Status of tasks with actual effort or invoiced budget cannot be changed back to "planned"
	if(($pda['aufwand_ist'] > 0.00) || ($pda['budget_ges_ext_abger'] > 0.00) || ($pda['budget_ges_int_verbr'] > 0.00) || ($pda['budget_ges_ext_verbr'] > 0.00) || strlen($pda['ist_beginn']))
		unset($Projekt_Status_array[FXP_PS_PLANNED]);

	// Status of tasks with planned but unapproved budget cannot be changed to "active" (as client setting)
	$ba_auto=strtolower(substr($GLOBALS['fxpglobals']['settings']['budgetgen'],0,1));
	$ba_active=strtolower(substr($GLOBALS['fxpglobals']['settings']['budgetgenaktiv'],0,1));
	if(($ba_auto == 'n') && ($ba_active == 'j') && ($vPST != FXP_PS_ACTIVE) && (($pda['budget_pers_int_gepl'] && !$pda['budget_pers_int_gen']) || ($pda['budget_mat_int_gepl'] && !$pda['budget_mat_int_gen'])))
	{
		$Projekt_Status_array[FXP_PS_ACTIVE]='<d>'.$Projekt_Status_array[FXP_PS_ACTIVE];
		$bt='';
		if($GLOBALS['fxpglobals']['lang'] == 1)
		{
			if($pda['budget_pers_int_gepl'] && !$pda['budget_pers_int_gen'])
				$bt='Personen (HR)';
			if($pda['budget_mat_int_gepl'] && !$pda['budget_mat_int_gen'])
			{
				if(strlen($bt))
					$bt .= ' und ';
				$bt .= 'Material';
			}
			$tt .= 'Aktivierung nicht möglich, da das geplante '.$bt.' Budget<br />noch <b>nicht</b> genehmigt wurde!';
		}
		else
		{
			if($pda['budget_pers_int_gepl'] && !$pda['budget_pers_int_gen'])
				$bt='HR';
			if($pda['budget_mat_int_gepl'] && !$pda['budget_mat_int_gen'])
			{
				if(strlen($bt))
					$bt .= ' and ';
				$bt .= 'MR';
			}
			$tt .= 'Activation not possible, because the planned '.$bt.' budget<br />has <b>not</b> been approved yet!';
		}
	}

	// Status of tasks with open incoming invoices cannot be changed to "finished"
	$sp="SELECT projekt_id FROM projekte WHERE vorgangsnummer LIKE '".$pda['vorgangsnummer']."%' AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
	$sql="SELECT COUNT(rechnungs_id) AS cnt FROM rechnungseingang WHERE projekt_id IN (".$sp.") AND nettobetrag>0 AND bezahlt_am IS NULL AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
	$ioc=(int)db_value($sql);
//fxDebug($ioc,$sql);
	if($ioc)
	{
		$Projekt_Status_array[FXP_PS_COMPLETED]='<d>'.$Projekt_Status_array[FXP_PS_COMPLETED];
		if(strlen($tt))
			$tt .= '<hr size=1>';
		if($GLOBALS['fxpglobals']['lang'] == 1)
			$tt .= 'Abschluss nicht möglich, da noch <b>offene</b><br />Eingansrechnungen ausstehen!';
		else
			$tt .= 'Completion not possible, because there are<br />still <b>open</b> incoming invoices outstanding!';
	}
}
$Projekt_Status_ds=0;
if(strlen($tt))
	$Projekt_Status_tt=$tt;
$maskenfeldwerte['projekt_status']=$vPST;

// Priorität (Prioritaet)
$Prioritaet_ds=0;
$maskenfeldwerte['prioritaet']=$vPRIO;

// Archiviert (projekt_archiviert)
$archived='';
if($err)
	$archived=$_POST['projekt_archiviert'];
else if($vMPID)
	$archived=db_value("SELECT projekt_archiviert FROM projekte WHERE projekt_id=".$vMPID." AND mandanten_id=".$GLOBALS['fxpglobals']['client']);
// ...nur für abgeschlossene Mutterprojekte kann das Archivierungskennzeichen gesetzt werden, falls entsprechendes Recht auf die Programmfunktion 20="Projekte archivieren" vorhanden ist
if(($GLOBALS['fxpglobals']['action'] != FXP_CHANGE) || ($vPART != FXP_PRJ_MAIN) || ($vPST != FXP_PS_COMPLETED) || !tr_rechte(20))
	$projekt_archiviert_dis=true;
$maskenfeldwerte['projekt_archiviert']=$archived;

// Freigabe (Freigabe)
if($app)
{
	$approval='';
	if($err)
	{
		if(isset($_POST['Freigabe']) && $_POST['Freigabe'])
			$approval='1';
	}
	else
		$approval=$vFREI;
	$maskenfeldwerte['freigabe']=$approval;
}
else
	$Freigabe_nv=true;

// Meilensteinname (Name_Meilenstein)
$Name_Meilenstein_nv=false;
if($GLOBALS['fxpglobals']['action'] == FXP_CREATE)
	$Name_Meilenstein_nv=true;
else
{
	$vNMST='';
	if($vAPID)
	{
		$sql="SELECT name_meilenstein, soll_ende FROM meilensteine WHERE projekt_id=".$vAPID." AND mandanten_id=".$GLOBALS['fxpglobals']['client']." ORDER BY soll_ende";
		$mstarr=db_values($sql,'*');
		if(fxIsArray($mstarr))
		{
			foreach($mstarr as $mstnr => $mstdat)
			{
				if($mstnr)
					$vNMST .= ', ';
				else
					$vNMST='~';
				$vNMST .= '<font class="darkgrey">'.get_entry($mstdat['soll_ende'], "datum").'</font>&nbsp;'.$mstdat['name_meilenstein'];
			}
		}
	}
	if(strlen($vNMST))
		$maskenfeldwerte['name_meilenstein']=$vNMST;
}

// Einsatzort (port)
$maskenfeldwerte['port']=$vPORT;

// Beschreibung (Beschreibung)
$maskenfeldwerte['beschreibung']=$vBESCH;

// HIDDEN (Projekt_ID, Mutterprojekt_ID, aktionscode)
$maskenfeldwerte['projekt_id']=$Projekt_ID;
$maskenfeldwerte['mutterprojekt_id']=$vMPID;
$maskenfeldwerte['aktionscode']=$sub_change;

// Kostenstammdaten
$kst_art=904;

// Funded projects (not supported yet, maybe in the future)
$fpp_nv=true;
$fpp_proj_nv=true;
$fpp_fproj_nv=true;
?>