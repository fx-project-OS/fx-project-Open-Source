<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : 158_ini.inc                                                  //
// Version     : 24.1                                                         //
// Begin       : 2020-08-27                                                   //
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
 * PF 158: Information - Display fx-project informations
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 24.1
 */


$act='';
if(fxIsArray($_GET) && isset($_GET['act']))
	$act=strtolower((string)trim((string)$_GET['act']));

$browser_text=fxf_detBrowser();
$browser_vers=(int)$GLOBALS['fxpglobals']['browservers'];
$logo=fxGetLogo('big');
$cvd=$GLOBALS['fxpglobals']['dbparam']['versionscode_db'];
$ivp=(int)$GLOBALS['fxpglobals']['dbparam']['version_patch'];
$cvp=substr('00'.$ivp,-3);

echo('<table border="0" cellspacing="0" cellpadding="0">');
echo('	<tr>');
echo('		<td valign="top"><div style="position:relative;"><div style="position:absolute;left:13px;top:12px;width:504px;height:504px;background:#fff;border-radius:50%;box-shadow:4px 4px 8px rgba(0,0,0, 0.5);"></div><img src="'.$logo.'" style="position:relative;left:0;top:0;"></div></td>');
echo('		<td width="5%">&nbsp;</td>');
echo('		<td width="95%" valign="top">');

// Invalid transaction
if(substr((string)$act,0,3) == 'itr')
{
	$itr=(int)substr((string)$act,3);

	$spp='';
	$spr=0;
	if(function_exists('fxsppCheckEBTR'))
		$spr=fxsppCheckEBTR($itr);
	$pft=loadMenu(true,false);
	if(fxIsArray($pft))
	{
		foreach($pft as $pf)
		{
			if($pf['tr'] == $itr)
			{
				if(strlen((string)$pf['pb']))
					$spp=$pf['pb'];
				break;
			}
		}
	}

	if(strlen((string)$spp) && !$spr)
	{
		if($GLOBALS['fxpglobals']['lang'] == 1)	// Deutsch
		{
			echo('			<font class="bold red">Folgende Programmfunktion wurde nicht gekauft oder nicht eingebunden:</font><br /><br /><b>'.$itr.':</b> '.trPath($itr).'<br />');
			if((int)$spp)
			{
				echo('<br /><hr class="fxhr"><br />');
				echo('			<b>Um diese Programmfunktion zu kaufen folgen Sie bitte dieser Anleitung:</b><br /><br />');
				echo('			<ul style="padding-left:10px;">');
				echo('				<li>Öffnen Sie in Ihrem Browser die Homepage des Herstellers. &rarr; '.($GLOBALS['_elinks']?'<a class="fxlink" href="https://www.fx-project.org" target="_blank">':'').'https://www.fx-project.org'.($GLOBALS['_elinks']?'</a>':'').'<br /><br /></li>');
				echo('				<li>Navigieren Sie zum <b>KAUF</b> Bereich</b>.</i><br /><br /></li>');
				echo('				<li>Wählen Sie den enstprechenden Programmzusatzblock zum Kaufen aus, hier: <b>Block '.$spp.'</b>. <i class="grey">(Angabe ohne Gewähr!)</i><br /><br /></li>');
				echo('			</ul>');
				echo('<br /><hr class="fxhr"><br />');
				echo('			<b>Um zu Überprüfen ob diese Programmfunktion korrekt eingebunden ist beachten Sie bitte folgende Punkte:</b><br /><br />');
				echo('			<ul style="padding-left:10px;">');
				echo('				<li>Im Unterordner &quot;<b>SPP</b>&quot; (Gekaufte Sonderprogramme) muss es einen Unterordner geben dessen Name mit &quot;'.substr('000'.$itr,-3).'&quot; beginnt.<br /><br /></li>');
				echo('				<li>Der o.g. Programmfunktionsunterordner &quot;'.substr('000'.$itr,-3).'&quot; darf nur einmal vorhanden sein.<br /><br /></li>');
				echo('			</ul>');
			}
		}
		else	// English
		{
			echo('			<font class="bold red">The following program function has not been purchased or integrated</font><br /><br /><b>'.$itr.':</b> '.trPath($itr).'<br />');
			if((int)$spp)
			{
				echo('<br /><hr class="fxhr"><br />');
				echo('			<b>To purchase this program function please follow these instructions:</b><br /><br />');
				echo('			<ul style="padding-left:10px;">');
				echo('				<li>Open the manufacturer\'s homepage in your browser. &rarr; '.($GLOBALS['_elinks']?'<a class="fxlink" href="https://www.fx-project.org" target="_blank">':'').'https://www.fx-project.org'.($GLOBALS['_elinks']?'</a>':'').'<br /><br /></li>');
				echo('				<li>Navigate to the <b>BUY</b> section.</i><br /><br /></li>');
				echo('				<li>Select the corresponding program add-on block to buy, here: <b>Block '.$spp.'</b>. <i class="grey">(Information without guarantee!)</i><br /><br /></li>');
				echo('			</ul>');
				echo('<br /><hr class="fxhr"><br />');
				echo('			<b>To check whether this program function is correctly integrated, please consider the following points:</b><br /><br />');
				echo('			<ul style="padding-left:10px;">');
				echo('				<li>In the subfolder &quot;<b>SPP</b>&quot; (Purchased Special Programs) there must be a subfolder whose name begins with &quot;'.substr('000'.$itr,-3).'&quot;.<br /><br /></li>');
				echo('				<li>The above-mentioned programme function subfolder &quot;'.substr('000'.$itr,-3).'&quot; may only exist once.<br /><br /></li>');
				echo('			</ul>');
			}
		}

		if($GLOBALS['fxpglobals']['lang'] == 1)	// Deutsch
		{
			echo('<br /><hr class="fxhr"><br />');
			echo('			<b>Diese Programmfunktion würde Ihnen folgende Funktionalitäten bieten:</b><br /><br />');
		}
		else	// English
		{
			echo('<br /><hr class="fxhr"><br />');
			echo('			<b>This programme function would offer you the following functionalities:</b><br /><br />');
		}
	}
	else if((!isset($GLOBALS['prgdir'][$itr]) && !isset($GLOBALS['sppdir'][$itr])) || (isset($GLOBALS['prgdir'][$itr]) && isset($GLOBALS['sppdir'][$itr])))
	{
		if($GLOBALS['fxpglobals']['lang'] == 1)	// Deutsch
		{
			echo('			<font class="bold red">Folgende Programmfunktion ist fehlerhaft eingebunden oder nicht vorhanden:</font><br /><br /><b>'.$itr.':</b> '.trPath($itr).'<br /><br /><hr class="fxhr"><br />');
			echo('			<b>Überprüfen Sie bitte folgende Punkte:</b><br /><br />');
			echo('			<ul style="padding-left:10px;">');
			echo('				<li>Im Unterordner &quot;<b>PRG</b>&quot; (Open Source Programme) oder &quot;<b>SPP</b>&quot; (Gekaufte Sonderprogramme) muss es einen Unterordner geben dessen Name mit &quot;'.substr('000'.$itr,-3).'&quot; beginnt.<br /><br /></li>');
			echo('				<li>Der o.g. Programmfunktionsunterordner &quot;'.substr('000'.$itr,-3).'&quot; darf nur einmal vorhanden sein.<br /><br /></li>');
			echo('			</ul>');
		}
		else	// English
		{
			echo('			<font class="bold red">The following program function is faulty integrated or not available</font><br /><br /><b>'.$itr.':</b> '.trPath($itr).'<br /><br /><hr class="fxhr"><br />');
			echo('			<b>Please check the following points:</b><br /><br />');
			echo('			<ul style="padding-left:10px;">');
			echo('				<li>In the subfolder &quot;<b>PRG</b>&quot; (Open Source Programs) or &quot;<b>SPP</b>&quot; (Purchased Special Programs) there must be a subfolder whose name begins with &quot;'.substr('000'.$itr,-3).'&quot;.<br /><br /></li>');
			echo('				<li>The above-mentioned programme function subfolder &quot;'.substr('000'.$itr,-3).'&quot; may only exist once.<br /><br /></li>');
			echo('			</ul>');
		}
	}
	else if($GLOBALS['fxpglobals']['lang'] == 1)	// Deutsch
	{
		echo('			<font class="bold red">Sie haben leider noch keine Berechtigung für folgende Programmfunktion</font><br /><br /><b>'.$itr.':</b> '.trPath($itr).'<br /><br /><hr class="fxhr"><br />');
		echo('			<b>Um diese Programmfunktion freizuschalten folgen Sie bitte dieser Anleitung:</b><br /><br />');
		echo('			<ul style="padding-left:10px;">');
		echo('				<li>Melden Sie sich als ein Benutzer an, welcher berechtigt ist administrative Tätigkeiten in fx-project auszuführen oder wenden Sie sich an Ihren Administrator.<br /><br /></li>');
		echo('				<li>Rufen Sie die Programmfunktion <font class="blue" style="cursor:pointer;" '.fxf_jsFunction('loadTR', '^loading', 97, '').'>'.trPath(97).'</font> auf, wählen Sie im Benutzer-Modus Ihren Benutzernamen aus. Dadurch erfahren Sie zu welcher Benutzerprofilgruppe Sie zugeordnet sind.<br /><br /></li>');
		echo('				<li>Rufen Sie die Programmfunktion <font class="blue" style="cursor:pointer;" '.fxf_jsFunction('loadTR', '^loading', 56, '').'>'.trPath(56).'</font> auf, wählen Sie Ihre Benutzerprofilgruppe aus und ordnen Sie die neue Programmfunktion zu.<br /><br /></li>');
		echo('			</ul>');
	}
	else	// English
	{
		echo('			<font class="bold red">Sorry, but you are not yet authorized for the following program function</font><br /><br /><b>'.$itr.':</b> '.trPath($itr).'<br /><br /><hr class="fxhr"><br />');
		echo('			<b>In order to activate this program function please follow this instruction:</b><br /><br />');
		echo('			<ul style="padding-left:10px;">');
		echo('				<li>Login as a user who is authorized to execute administrative operations in fx-project or contact your administrator.<br /><br /></li>');
		echo('				<li>Call the program function <font class="blue" style="cursor:pointer;" '.fxf_jsFunction('loadTR', '^loading', 97, '').'>'.trPath(97).'</font> and select your user name in the user mode, thus you find out to which user profile group you are assigned to.<br /><br /></li>');
		echo('				<li>Call the program function <font class="blue" style="cursor:pointer;" '.fxf_jsFunction('loadTR', '^loading', 56, '').'>'.trPath(56).'</font>, select your user profile group and assign the new program function.<br /><br /></li>');
		echo('			</ul>');
	}

	// Display functionality
	$filepath=$GLOBALS['docpath'].substr('00'.(int)$GLOBALS['fxpglobals']['lang'],-2).'/';
	$filename=$filepath.substr('000000'.$itr,-6).'.htm';
	if(file_exists($filename))
	{
		$html=strtr(getHelpText($filename,true,'width:664px;'), array('"../../'=>'"./'));
		echo($html);
	}

	echo('		</td>');
}
else
{
	echo('			<table width="100%" border="0" cellspacing="0" cellpadding="16">');

	// Current version + patch
	$pla=explode('|', meldung(3382));	// Version|No Version|Patch|No Patch
	if(function_exists('fxsppSetVersion'))
		fxsppSetVersion();
	echo('				<tr>');
	echo('					<td colspan="2" valign="top">');
	echo('						<h1 class="tshadow">fx-project '.$v_type.' Edition V'.$v_fxp.'</h1>');
	if($ivp)	// Patch
		echo('						'.$pla[2].' '.$ivp.' <i>('.get_entry($GLOBALS['fxpglobals']['dbparam']['date_patch'], 'datum').')</i>');
	else		// No Patch
		echo('						<i class="grey">('.$pla[3].')</i>');
	echo('					</td>');
	echo('				</tr>');

	// Users + Browser
	echo('				<tr>');
	// ...Users
	echo('					<td width="50%" valign="top">');
	$oid=substr('000000'.$GLOBALS['fxpglobals']['client'],-6).'|'.$GLOBALS['fxpglobals']['pname'].'|'.$GLOBALS['fxpglobals']['user'].'|'.$GLOBALS['fxpglobals']['person'];	// ClientID+PersonName+UserID+PersonID
//fxDebug($oid,'$oid');
	$uarr=array($oid => array('m'=>$GLOBALS['fxpglobals']['client'], 'u'=>$GLOBALS['fxpglobals']['uname'], 'n'=>$GLOBALS['fxpglobals']['pname'], 'l'=>array($GLOBALS['lts']=>true)));
	$u=usrLog();
//fxDebug($u,'$u');
	if(fxIsArray($u))
	{
		foreach($u as $id => $d)
		{
			$ds=(int)$datas[0];		// Unix timestamp
			$dt=(string)$datas[1];	// FeRox timestamp
			$dc=(int)$datas[2];		// Client ID
			$du=(int)$datas[3];		// User ID
			$dp=(int)$datas[4];		// Person ID
			$di=(int)$datas[5];		// Transaction ID
			$dl=(string)$datas[6];	// Login timestamp

			if(!$GLOBALS['fxpglobals']['person'] || ($d['client'] == $GLOBALS['fxpglobals']['client']))
			{
				$cup=$d['client'].'-'.$d['user'].'-'.$d['person'];
				if(!isset($cupa[$cup]))
				{
					$sql="SELECT b.name_benutzer, p.pname,p.vorname FROM benutzer b, personen p WHERE b.benutzer_id=".$d['user']." AND b.mandanten_id=".$d['client']." AND b.personen_id=p.personen_id AND p.personen_id=".$d['person']." AND p.mandanten_id=".$d['client'];
					$tmp=db_values($sql);
//fxDebug($tmp,$sql);
					if(fxIsArray($tmp))
					{
						$n=$tmp['pname'];
						if(strlen((string)$tmp['vorname']))
							$n .= ', '.$tmp['vorname'];

						$cupa[$cup]=array('m'=>$d['client'], 'u'=>strtolower((string)$tmp['name_benutzer']), 'n'=>$n, 'l'=>array($d['lts']=>true));
					}
					else
						$cupa[$cup]=array('m'=>$d['client'], 'u'=>"[".$d['user']."]", 'n'=>"[".$d['person']."]", 'l'=>array($d['lts']=>true));
				}

				$dn=$cupa[$cup]['n'];

				// ID
				$id=substr('000000'.$d['client'],-6).'|'.$dn.'|'.$d['user'].'|'.$d['person'];	// ClientID+PersonName+UserID+PersonID
				if(!isset($uarr[$id]))
					$uarr[$id]=$cupa[$cup];
				else
					$uarr[$id]['l'][$d['lts']]=true;
			}
		}
	}
	if($GLOBALS['fxpglobals']['lang'] == 1)
		$lit='Benutzer Online';
	else
		$lit='Users Online';
	$uc=0;
	foreach($uarr as $id => $u)
		$uc += sizeof($u['l']);
	echo('						<br /><hr class="fxhr" style="border-top:1px solid #dddddd;"><span class="lit">'.$lit.':</span> '.$uc.'<br /><br />');

	$euo=(int)substr((string)$GLOBALS['fxpglobals']['settings']['users'], 0, 1);
	if(($euo < 2) && fxIsArray($uarr))
	{
		ksort($uarr);
//fxDebug($uarr,'$uarr');
		echo('						<table width="100%" border="0" cellspacing="0" cellpadding="0">');
		// This user
		echo('							<tr>');
		foreach($uarr as $id => $u)
		{
			if($id == $oid)
			{
				echo('								<td nowrap><font class="grey">'.$u['m'].'</font></td><td nowrap>&nbsp;&nbsp;<b>'.$u['u'].'</b>&nbsp;&nbsp;</td><td class="blue" nowrap>'.$u['n'].'</td><td nowrap width="100%">&nbsp;&nbsp;');
				$tuc=sizeof($u['l']);
				if($tuc > 1)
					echo('<i class="lightergrey">('.$tuc.'x)');
				echo('</td>');
			}
		}
		echo('							</tr>');
		// Other users
		$uc=0;
		foreach($uarr as $id => $u)
		{
			if($id != $oid)
			{
				if(!$uc)
				{
					echo('							<tr>');
					echo('								<td colspan="4"><br /></td>');
					echo('							</tr>');
				}
				echo('							<tr>');
				echo('								<td nowrap><font class="grey">'.$u['m'].'</font></td><td nowrap>&nbsp;&nbsp;<b>'.$u['u'].'</b>&nbsp;&nbsp;</td><td nowrap>'.$u['n'].'</td><td nowrap width="100%">&nbsp;&nbsp;');
				$tuc=sizeof($u['l']);
				if($tuc > 1)
					echo('<i class="lightergrey">('.$tuc.'x)');
				echo('</td>');
				echo('							</tr>');
				$uc++;
			}
		}
		echo('						</table>');
	}
	echo('					</td>');
	// ...Browser
	echo('					<td width="50%" valign="top">');
	echo('						<br /><hr class="fxhr" style="border-top:1px solid #dddddd;"><span class="lit">Browser:</span><br /><br />');
	echo('						<b>'.$browser_text.'</b>');
	echo('					</td>');
	echo('				</tr>');

	// Documents
	$docs=array(
		1 => array('Dokumente', 'Sie arbeiten zum ersten Mal mit fx-project?<br />Dann lesen Sie sich bitte folgende Dokumente durch:', 'fx-project 00.1 Kurzuebersicht.pdf', 'fx-project 05.1 Erste Schritte.pdf'),
		7 => array('Documents', 'You are working for the first time with fx-project?<br />Please read the following documents:', 'fx-project 00.1 Brief Overview.pdf', 'fx-project 05.1 First Steps.pdf')
	);
	$ltxt=trim((string)strtoupper(meldung($GLOBALS['fxpglobals']['lang'], false)));
	$dcont  = '				<tr>';
	$dcont .= '					<td colspan="2" valign="top">';
	$dcont .= '						<hr class="fxhr" style="border-top:1px solid #dddddd;">';
	if($act == 'help')
		$dcont .= '<b class="red">'.$docs[$GLOBALS['fxpglobals']['lang']][1].'</b>';
	else
		$dcont .= '<span class="lit">'.$docs[$GLOBALS['fxpglobals']['lang']][0].':</span>';
	$dcont .= '<br /><br />';
	$dcnt=0;
	if(fxIsArray($docs[$GLOBALS['fxpglobals']['lang']]))
	{
		foreach($docs[$GLOBALS['fxpglobals']['lang']] as $dc => $di)
		{
			if($dc > 1)
			{
				$dpath=$GLOBALS['docpath'].$ltxt.'/';
				if(!file_exists($dpath.$di))
				{
					$dpath=$GLOBALS['docpath'];
					if(!file_exists($dpath.$di))
						$dpath='';
				}

				if(strlen((string)$dpath))
				{
					$br='';
					if($dcnt && (substr((string)$di,11,2) != '00'))
						$br='<br />';
					$dcont .= $br.'						<img src="'.$GLOBALS['gfxpath'].'ico_pdf.png" align="top"" style="margin-bottom:4px;">&nbsp;&nbsp;<a class="fxlink" href="'.$dpath.$di.'" target="_blank">'.substr((string)$di, 16, -4).'</a><br />';
					$dcnt++;
				}
			}
		}
	}
	$dcont .= '					</td>';
	$dcont .= '				</tr>';
	if($dcnt)
		echo($dcont);

	// Graphics, Images + Icons
	if($GLOBALS['fxpglobals']['lang'] == 1)	// Deutsch
	{
		$headline='Grafiken, Bilder + Icons';
		$text='Alle Grafiken, Bilder und Icons wurden entweder vom Hersteller der Software selbst erstellt, oder sind gekaufte und lizensierte Grafiken.';
		$license='Lizenz';
		$tha=array('Author', 'Link', 'Lizenz');
	}
	else	// English
	{
		$headline='Graphics, Images + Icons';
		$text='All graphics, images and icons were either created by the manufacturer of the software itself, or are purchased and licensed graphics.';
		$license='License';
		$tha=array('Author', 'Link', 'License');
	}
	echo('				<tr>');
	echo('					<td colspan="2" valign="top">');
	echo('						<hr class="fxhr" style="border-top:1px solid #dddddd;"><span class="lit">'.$headline.':</span><br /><br />');
	echo('						'.$text.'<br /><br />');
	echo('						<table width="100%" border="0" cellspacing="2" cellpadding="4">');
	echo('							<tr>');
	echo('								<td valign="top" bgcolor="#eeeeee" nowrap>'.$tha[0].'</td>');					// Author
	echo('								<td valign="top" bgcolor="#eeeeee" nowrap>'.$tha[1].'</td>');					// Link
	echo('								<td valign="top" bgcolor="#eeeeee" nowrap>'.$tha[2].'</td>');					// License
	echo('							</tr>');
	echo('							<tr>');
	echo('								<td valign="top" nowrap><b>FeRox Management Consulting GmbH & Co. KG</b></td>');
	echo('								<td valign="top" nowrap>&rarr; '.($GLOBALS['_elinks']?'<a class="fxlink" href="https://www.ferox.de" target="_blank">':'').'https://www.ferox.de'.($GLOBALS['_elinks']?'</a>':'').'</td>');
	echo('								<td valign="top" nowrap>GNU-GPL v3</td>');
	echo('							</tr>');
	echo('							<tr>');
	echo('								<td valign="top" nowrap><b>Shock Family</b></td>');
	echo('								<td valign="top" nowrap>&rarr; '.($GLOBALS['_elinks']?'<a class="fxlink" href="https://www.iconshock.com" target="_blank">':'').'https://www.iconshock.com'.($GLOBALS['_elinks']?'</a>':'').'</td>');
	echo('								<td><img src="'.$GLOBALS['gfxpath'].'ico_pdf.png" align="top" style="margin-bottom:4px;">&nbsp;<a class="fxlink" href="'.$GLOBALS['gfxpath'].'_SHOCK_FAMILY_Standard_License_Commercial.pdf" target="_blank">'.$license.'</a></td>');
	echo('							</tr>');
	echo('						</table>');
	echo('					</td>');
	echo('				</tr>');

	// Programs
	$pa=array(
		'ChartDirector'=>array('r'=>'ext_chartdir', 'e'=>'_ext_chartdir', 'l'=>'https://www.advsofteng.com', 'v'=>'7.1', 'd'=>'2022-12-24', 'a'=>'Advanced Software Engineering Ltd', 't'=>'<i class="red">(Trial)</i>'),
		'FPDF'=>array('r'=>'ext_pdf', 'e'=>'_ext_pdf', 'l'=>'http://www.fpdf.org', 'v'=>'1.86', 'd'=>'2023-06-25', 'a'=>'Olivier PLATHEY', 't'=>'<i class="green">(Free)</i>'),
		'FPDI'=>array('r'=>'ext_pdf', 'e'=>'_ext_fpdi', 'l'=>'https://www.setasign.com', 'v'=>'2.6.0', 'd'=>'2023-12-11', 'a'=>'Setasign GmbH & Co. KG', 't'=>'MIT'),
		'PHPMailer'=>array('r'=>'ext_mailer', 'e'=>'_ext_mailer', 'l'=>'https://github.com/PHPMailer/PHPMailer', 'v'=>'6.9.1', 'd'=>'2023-11-25', 'a'=>'Marcus Bointon, Jim Jagielski + Andy Prevost', 't'=>'LGPL-2.1'),
//		'PhpSpreadsheet'=>array('r'=>'ext_spreadsheet', 'e'=>'_ext_spreadsheet', 'l'=>'https://github.com/PHPOffice/PhpSpreadsheet', 'v'=>'2.0.0', 'd'=>'2024-01-24', 'a'=>'<i>Various</i>', 't'=>'MIT'),
		'TCPDF'=>array('r'=>'ext_pdf', 'e'=>'_ext_pdf', 'l'=>'https://tcpdf.org', 'v'=>'6.4.4', 'd'=>'2022-08-12', 'a'=>'Nicola Asuni - Tecnick.com LTD', 't'=>'LGPL-3')
	);
	if($GLOBALS['fxpglobals']['lang'] == 1)	// Deutsch
	{
		$headline='Optionale Externe Programme';
		$text='fx-project verwendet folgende externe Programme, welche im &quot;EXT&quot; Unterordner unverändert zu finden sind.';
		$tha=array('Programm', 'Info', 'Link', 'Version', 'Datum', 'Author', 'Lizenz', 'Status');
		$pai=array(
			'ChartDirector'=>'<b>Bibliothek für Diagramm- und Kurvendiagramme.</b><br /><br /><hr size=1 /><br />fx-project benutzt diese PHP-Erweiterungsbibliothek (Extension) um Diagramme<br />und Kurvendiagramme zu erzeugen und anzuzeigen.<br /><br />Eine nicht gekaufte Version wird als Testversion behandelt, d.h. jedes erzeugte<br />Diagramm hat am unteren Rand einen Hinweis des Herstelles. Ein evtl. gekaufter<br />ChartDirector-Schlüssel kann zum Freischalten in den Unterordner<br />&quot;EXT/CHARTDIRECTOR&quot; kopiert werden als Textdatei &quot;phpchartdir.ini&quot;.<br />Der Inhalt einer solchen Datei wird automatisch von fx-project verschlüsselt.<br /><br />Jedes gekaufte Erweiterungsmodul des Herstellers von fx-project wandelt ebenfalls<br />eine Testversion in eine Vollversion um, ohne dass ein separater Schlüssel gekauft<br />werden muss.<br /><br /><span class=red>Ist diese PHP-Extension nicht vorhanden werden KEINE Diagramme erzeugt!</span>',
			'FPDF'=>'<b>Free PDF: Eine freie PHP-Klasse zum Erzeugen von PDF\'s (Portable Document Format).</b><br /><br /><hr size=1 /><br />fx-project benutzt diese PHP-Klasse um die erzeugten und angezeigten HTML-Reports in ein<br />downloadbares Format (PDF) umzuwandeln, welches einheitlich angezeigt und auf jedem<br />Drucker ausgedruckt werden kann.<br /><br /><span class=red>Ist diese PHP-Klasse nicht vorhanden wird als Alternative die PHP-Klasse &quot;TCPDF&quot; verwendet,<br />falls vorhanden. Ist keine dieser beiden PHP-Klassen vorhanden werden KEINE PDF\'s erzeugt!</span>',
			'FPDI'=>'<b>Free PDF Document Importer: Eine freie PHP-Klasse zum Einlesen und Analysieren<br />von bestehenden PDF-Dokumenten (Portable Document Format).</b><br /><br /><hr size=1 /><br />fx-project benutzt diese PHP-Klasse um eine Vorschau der ersten Seite eines PDF-Dokuments<br />im DMS (Document Management System) zu erzeugen und anzuzeigen.<br /><br />FPDI ist eine Erweiterung und benötigt als Basis die FPDF- oder die TCPDF-Klasse.<br /><br /><span class=red>Ist diese PHP-Klasse nicht vorhanden wird im DMS keine Seitenvorschau angezeigt!</span>',
			'PHPMailer'=>'<b>Eine voll funktionsfähige E-Mail-Erstellungs- und Übertragungsklasse für PHP.</b><br /><br /><hr size=1 /><br />fx-project benutzt diese PHP-Klasse um HTML-E-Mails zu erstellen, welche mittels<br />SMTP und einer Socket-Verbindung verschickt werden können.<br /><br /><span class=red>Ist diese PHP-Klasse nicht vorhanden wird als Alternative die Standard-PHP-Funktion<br />&quot;mail&quot; verwendet!</span>',
			'PhpSpreadsheet'=>'<b>Eine Reihe von PHP-Klassen mit denen verschiedene Tabellenkalkulationsdateiformate<br />wie Excel und LibreOffice Calc gelesen und geschrieben werden können.</b><br /><br /><hr size=1 /><br />fx-project benutzt diese PHP-Klasse um die erzeugten und angezeigten HTML-Reports in<br />downloadbare Excel-Dateien umzuwandeln, welche mit einer entsprechenden Software<br />geladen, angezeigt und weiterbearbeitet werden können.<br /><br /><span class=red>Ist diese PHP-Klasse nicht vorhanden werden keine Excel-Dateien erzeugt!</span>',
			'TCPDF'=>'<b>Tecnick PDF: Eine freie PHP-Klasse zum Erzeugen von PDF\'s (Portable Document Format).</b><br /><br /><hr size=1 /><br />fx-project benutzt diese PHP-Klasse um die erzeugten und angezeigten HTML-Reports in ein<br />downloadbares Format (PDF) umzuwandeln, welches einheitlich angezeigt und auf jedem<br />Drucker ausgedruckt werden kann.<br /><br /><span class=red>Ist diese PHP-Klasse nicht vorhanden wird als Alternative die PHP-Klasse &quot;FPDF&quot; verwendet,<br />falls vorhanden. Ist keine dieser beiden PHP-Klassen vorhanden werden KEINE PDF\'s erzeugt!</span>'
		);
	}
	else	// English
	{
		$headline='Optional External Programs';
		$text='fx-project uses the following external programs, which can be found unchanged in the &quot;EXT&quot; subfolder.';
		$tha=array('Program', 'Info', 'Link', 'Version', 'Date', 'Author', 'License', 'Status');
		$pai=array(
			'ChartDirector'=>'<b>Chart and Graph Plotting Library.</b><br /><br /><hr size=1 /><br />fx-project uses this PHP extension library to generate and display charts and<br />graph plotting diagrams.<br /><br />A version that has not been purchased is treated as a test version, i.e. every chart<br />created has a manufacturer\'s reference at the bottom. A possibly purchased<br />ChartDirector key can be copied into the subfolder &quot;EXT/CHARTDIRECTOR&quot; as a<br />text file &quot;phpchartdir.ini&quot; for unlocking. The content of such a file is automatically<br />encrypted by fx-project.<br /><br />Each purchased extension module from the manufacturer of fx-project also converts<br />a test version into a full version, without the need to purchase a separate key.<br /><br /><span class=red>If this PHP-Extension is not present NO charts will be created!</span>',
			'FPDF'=>'<b>Free PDF: A free PHP class for generating PDF\'s (Portable Document Format).</b><br /><br /><hr size=1 /><br />fx-project uses this PHP class to convert the generated and displayed HTML reports<br />into a downloadable format (PDF), which can be displayed uniformly and printed on<br />any printer.<br /><br /><span class=red>If this PHP class is not available, the PHP class &quot;TCPDF&quot; is used as an alternative,<br />if available. If neither of these PHP classes are available, NO PDF\'s are generated!</span>',
			'FPDI'=>'<b>Free PDF Document Importer: A free PHP class for reading and analysing<br />existing PDF (Portable Document Format) documents.</b><br /><br /><hr size=1 /><br />fx-project uses this PHP class to generate and display a preview of the first page of<br />a PDF document in the DMS (Document Management System).<br /><br />FPDI is an extension and requires the FPDF or TCPDF class as a basis.<br /><br /><span class=red>If this PHP class is not available, no page preview will be displayed in the DMS!</span>',
			'PHPMailer'=>'<b>A full-featured email creation and transfer class for PHP.</b><br /><br /><hr size=1 /><br />fx-project uses this PHP class to create HTML emails, which can be<br />sent via SMTP and a socket connection.<br /><br /><span class=red>If this PHP class is not available, the standard PHP function<br />&quot;mail&quot; is used as an alternative!</span>',
			'PhpSpreadsheet'=>'<b>A set of PHP classes with which various spreadsheet file formats<br />such as Excel and LibreOffice Calc can be read and written.</b><br /><br /><hr size=1 /><br />fx-project uses this PHP class to convert the generated and displayed HTML<br />reports into downloadable Excel files, which can be loaded, displayed and<br />further processed with appropriate software.<br /><br /><span class=red>If this PHP class is not present no Excel files will be generated!</span>',
			'TCPDF'=>'<b>Tecnick PDF: A free PHP class for generating PDF\'s (Portable Document Format).</b><br /><br /><hr size=1 /><br />fx-project uses this PHP class to convert the generated and displayed HTML reports<br />into a downloadable format (PDF), which can be displayed uniformly and printed on<br />any printer.<br /><br /><span class=red>If this PHP class is not available, the PHP class &quot;FPDF&quot; is used as an alternative,<br />if available. If neither of these PHP classes are available, NO PDF\'s are generated!</span>'
		);
	}
	echo('				<tr>');
	echo('					<td colspan="2" valign="top">');
	echo('						<hr class="fxhr" style="border-top:1px solid #dddddd;"><span class="lit">'.$headline.':</span><br /><br />');
	echo('						'.$text.'<br /><br />');
	echo('						<table width="100%" border="0" cellspacing="2" cellpadding="4">');
	echo('							<tr>');
	echo('								<td valign="top" bgcolor="#eeeeee" nowrap>'.$tha[0].'</td>');					// Program
	echo('								<td align="center" valign="top" bgcolor="#eeeeee" nowrap>'.$tha[1].'</td>');	// Info
	echo('								<td valign="top" bgcolor="#eeeeee" nowrap>'.$tha[2].'</td>');					// Link
	echo('								<td align="right" valign="top" bgcolor="#eeeeee" nowrap>'.$tha[3].'</td>');		// Version
	echo('								<td align="center" valign="top" bgcolor="#eeeeee" nowrap>'.$tha[4].'</td>');	// Date
	echo('								<td valign="top" bgcolor="#eeeeee" nowrap>'.$tha[5].'</td>');					// Author
	echo('								<td valign="top" bgcolor="#eeeeee" nowrap>'.$tha[6].'</td>');					// License
	echo('								<td align="center" valign="top" bgcolor="#eeeeee" nowrap>'.$tha[7].'</td>');	// Status
	echo('							</tr>');
	foreach($pa as $pn => $paa)
	{
		// Check extension status
		$st='<img src="'.$GLOBALS['gfxpath'].'tl_red.png">';
		if(strlen((string)$paa['r']))
		{
			if($pn == 'FPDI')
				$GLOBALS['_ext_pdf_fpdi']=true;
			else if($pn == 'TCPDF')
				$GLOBALS['_ext_pdf_tcpdf']=true;
			$req_fn=fxf_fn_reqFilename($paa['r']);
			if(strlen((string)$req_fn))
				require($req_fn);
			if(is_string($GLOBALS[$paa['e']]) && strlen((string)$GLOBALS[$paa['e']]))
			{
				$st='<img src="'.$GLOBALS['gfxpath'].'tl_green.png">';
				if($pn == 'ChartDirector')
				{
					if(!strlen((string)$_ext_chartdir_array['cdok']) || (!strlen((string)$_ext_chartdir_array['cdkey']) && (substr((string)$v_type,0,1) == 'C')))
						$st='<img src="'.$GLOBALS['gfxpath'].'tl_yellow.png">';
					else
						$paa['t']='<i class="green">(Key)</i>';
				}
			}
		}
		echo('							<tr>');
		echo('								<td valign="top" nowrap><b>'.$pn.'</b></td>');
		echo('								<td align="center" valign="top" nowrap><img src="'.$GLOBALS['gfxpath'].'hlp_b_21x21.png" width="16" height="16" tooltip="'.$pai[$pn].'<br /><br /><hr size=1 /><br /><div style=\'text-align:right;\'><b>'.$tha[7].':</b>&nbsp;&nbsp;'.str_replace('"', '\'', $st).'</div>" style="cursor:help;"></td>');
		echo('								<td valign="top" nowrap>&rarr; '.($GLOBALS['_elinks']?'<a class="fxlink" href="'.$paa['l'].'" target="_blank">':'').$paa['l'].($GLOBALS['_elinks']?'</a>':'').'</td>');
		echo('								<td align="right" valign="top" nowrap>'.$paa['v'].'</td>');
		echo('								<td align="center" valign="top" nowrap>'.$paa['d'].'</td>');
		echo('								<td valign="top" nowrap>'.$paa['a'].'</td>');
		echo('								<td valign="top" nowrap>'.$paa['t'].'</td>');
		echo('								<td align="center" valign="top" nowrap>'.$st.'</td>');
		echo('							</tr>');
	}
	echo('						</table>');
	echo('					</td>');
	echo('				</tr>');

	echo('			</table>');
	echo('		</td>');
}
echo('	</tr>');
echo('</table>');
?>