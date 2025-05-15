////////////////////////////////////////////////////////////////////////////////
// File name   : fxp.js                                                       //
// Version     : 24.1                                                         //
// Begin       : 2020-07-23                                                   //
// Last Change : 2024-08-23                                                   //
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
 * JavaScript main function collection
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 24.1
 */

// Globals
var oFXP={};	// FXP object... contains all relevant variables for fx-project - like language, transaction id etc.
var oSet={};	// Setting object... contains common settings
var gSet={};	// Global setting object... contains globals settings like help
var tSet={};	// Transaction setting object... contains special settings for actual transaction
var oID={};		// ID object... contains div id's of all elements
var oDebug={};	// Debug object... contains div id's of debug window or debug information
var oVar={};	// Var object... contains field informations for all masks
var sunit={};	// Store unit fields in seconds
var dim={};		// Screen and work dimensions

var aTXT=[];	// Text array

var aEl=null;
var fEl=null;

// Keep User Alive (in seconds, 0=no)?
//var jsKUA=0;
var jsKUA=10.0;

// Asynchronous AJAX calls?
var asajax=true;

// Delays
oFXP.sdelay=0.25;
oFXP.mdelay=0.50;
oFXP.ldelay=1.0;

// List headers
oFXP.listha=[];

// Matrix
oID.sdg_container=null;
oID.structure=null; oID.sstructure=null; oID.detail=null; oID.sdetail=null; oID.gantt=null; oID.sgantt=null;
oID.sd_divider=null; oID.dg_divider=null;
oID.shead=null; oID.smain=null; oID.slmarker=null; oID.smarker=null; oID.swork=null; oID.saction=null;
oID.dhead=null; oID.dheaders=null; oID.dheaderlits=null; oID.dmain=null; oID.dlmarker=null; oID.dmarker=null; oID.dwork=null; oID.dwdata=null;
oID.gtime=null; oID.gmain=null; oID.gaction=null; oID.gdaction=null; oID.gdepsi=null; oID.gdepdi=null;
oID.time0=null; oID.time1=null; oID.time2=null; oID.tmarker=null;
oID.markers=null; oID.gmarker=null; oID.gmarkert=null; oID.gmarkerb=null; oID.gmarkerl=null; oID.gmarkerr=null;
oID.gmove=null; oID.gmovet=null; oID.gmovef=null; oID.gdep=null; oID.gdepi=null; oID.gdept=null; oID.gbars=null;
oID.gjumps=null; oID.gleft1=null; oID.gleft2=null; oID.gright2=null; oID.gright1=null; oID.gjumpe=null;
oID.overview=null; oID.toverviewd=null; oID.toverviewi=null; oID.doverview=null; oID.coverview=null;
oID.sshrink=null; oID.dshrink=null; oID.gshrink=null;
oID.mindatarea=null; oID.maxdatarea=null;
oID.minmgarea=null; oID.maxmgarea=null;
oID.ipdata=null; oID.ipprojectname=null; oID.iptimespan=null; oID.ipduration=null; oID.ipplaneffort=null; oID.ipacteffort=null; oID.remeffort=null; oID.ipresources=null; oID.ipstatus=null;

oID.ndw_container=null;
oID.hrbplan=null; oID.hrbwload=null;
oID.hrnames=null; oID.hrdata=null; oID.hrworkload=null; oID.hrinfo=null;
oID.mh_divider=null; oID.nd_divider=null; oID.dw_divider=null;
oID.nhead=null; oID.nmain=null; oID.nwork=null;
oID.aheaders=null; oID.aheaderlits=null; oID.amain=null; oID.awork=null; oID.awdata=null;
oID.wtime=null; oID.wtime2=null; oID.wtmarker=null; oID.wmain=null; oID.wbars=null; oID.wmarkers=null;
oID.minwgarea=null; oID.maxwgarea=null;
oID.hrpool=null;

var mcolumns=[];
var rcolumns=[];
var selentries=[];
var mselinfo={'mode':-1, 'element':null, 'entries':null, 'dim':null};

// Timeregistration Week
oID.trw_container=null;
oID.trtmain=null; oID.trimain=null; oID.trinfo=null; oID.trsinfo=null; oID.trentries=null; oID.trehead=null; oID.tremain=null; oID.trsum=null; oID.trsmain=null;

var mcx=0, mcy=0, rmcx=0, rmcy=0, rscl=0, rsct=0;
var minleft=0, maxright=0, pmindate='', pmaxdate='';
var dh=24, lth=dh-8, lph=dh-9, xd=0, mcl=0, mdp='';
var mmode=0, mcheck=0, cmenu=0, mka='', cmd='';
var scx=0, scxr=0, scy=0;
var rsl=-1, rgl=-1, soi='', soii=-1, moi='', moii=-1;
var cts=0, selsc=0, pcc=false;
var uid=1;
var achg=false;
var fwms=0;

// Global settings
gSet.maintop=0;
gSet.filter=1;
gSet.help=1;
gSet.trmark=1;
gSet.alignworkday=0;
gSet.showframes=1;
gSet.showlines=0;
gSet.showhr=1;
gSet.contnumbers=0;
gSet.ppsppopalways=0;
gSet.dunit=2;	// Duration unit: 1=Min, 2=Hrs, 3=PD, 4=PW, 5=PM + 6=PY
gSet.cunit=false;
gSet.dbgl=0;
gSet.wfsl=0;
gSet.prjsl=0;
gSet.maxfloat=999999999999999.0;
gSet.wfradius=0;
gSet.wfsissi='';

var scrshtmode=-1;

var slarge=true;
var dlarge=true;
var glarge=true;
var hlarge=false;

var adisp=true;

var srwidth=256;
var drwidth=256;
var grwidth=0;
var hrheight=0;

var tbswidth=128;
var tblwidth=256;

var dateElem=null;
var emode=null;
var editID=null;
var aKeys=true;
var imode='';
var klt=false;
var kesc=0;

var minas=144;
var minag=324;

var workweek=[0,1,1,1,1,1,0];	// Sunday to Saturday

var zoom_off=13;
var zoom_dist=14;
var sdate='', edate='';
var mdays=0, mlines=0;
var swd=0;
var undop=0, undoc=0;

var prohibitChanges=false;

var drag_object={};
var hexstr='0123456789abcdef';
var divstr=['!-0-!', '!-1-!', '!-2-!'];
var zdays=[1,2,4,6,8,12,16,32,48,64];
var days=[];

var pArray=[];	// Projects
var lArray=[];	// ...left position
var wArray=[];	// ...width
var eArray=[];	// ...resources

var rArray=[];	// Resources
var aArray=[];	// Assigned resources

var dArray=[];	// Dependencies

var hArray=[];	// Holidays

var pAct=-1, pRem=-1;	// Project marker
var dAct=-1, dRem=-1;	// Dependency marker

var debug=false;

var zObject=null;
var doi={};
var dstack=[];	// Deadlines stack

var fstack=[];	// Function call stack

var eventListeners=[];
var actEvent=null;
var keyEvent=null;

// Prototypes
String.prototype.getLevel=function()
{
	return (this.length - this.replace(/\./g, '').length);
};
Element.prototype.trigger=function(eventName)
{
	if(document.createEvent)
	{
		var evt=document.createEvent('HTMLEvents');
		evt.initEvent(eventName, true, true);
		return this.dispatchEvent(evt);
	}
	if(this.fireEvent)
		return this.fireEvent('on'+eventName);
};

// Browser type and version
var ual=navigator.userAgent.toLowerCase();
Prototype.BrowserFeatures['Type']='Unknown';
Prototype.BrowserFeatures['Version']=0;
Prototype.BrowserFeatures['Mobile']='';
if(ual.indexOf('trident') > 0)
	Prototype.Browser.IE=true;

// Edge (Chromium): mozilla/5.0 (windows nt 10.0; win64; x64) applewebkit/537.36 (khtml, like gecko) chrome/80.0.3987.163 safari/537.36 edg/80.0.361.111
if(/edg\/(\d+\.\d+)/.test(ual))
{
	Prototype.Browser.Edge=true;
	Prototype.BrowserFeatures['Type']='Microsoft Edge';
   	Prototype.BrowserFeatures['Version']=new Number(RegExp.$1);
}
// Edge (Internet Explorer): mozilla/5.0 (windows nt 10.0) applewebkit/537.36 (khtml, like gecko) chrome/42.0.2311.135 safari/537.36 edge/12.10162
else if(/edge\/(\d+\.\d+)/.test(ual))
{
	Prototype.Browser.Edge=true;
	Prototype.BrowserFeatures['Type']='Internet Explorer: Edge';
   	Prototype.BrowserFeatures['Version']=new Number(RegExp.$1);
}
// IE=Internet Explorer: Mozilla/4.0 (compatible; MSIE 7.0; Windows NT 6.3; Trident/7.0)
else if(/msie (\d+\.\d+);/.test(ual))
{
	Prototype.Browser.IE=true;
	Prototype.BrowserFeatures['Type']='Internet Explorer';
   	Prototype.BrowserFeatures['Version']=new Number(RegExp.$1);
}
// IE=Internet Explorer: Mozilla/5.0 (Windows NT 6.1; Trident/7.0; rv:11.0) like Gecko
else if(/trident\/(\d+\.\d+)/.test(ual))
{
	Prototype.Browser.IE=true;
	Prototype.BrowserFeatures['Type']='Internet Explorer';
	if(/rv\:(\d+\.\d+)\)/.test(ual))
	   	Prototype.BrowserFeatures['Version']=new Number(RegExp.$1);
}
// Firefox: Mozilla/5.0 (Windows NT 6.0; WOW64; rv:26.0) Gecko/20100101 Firefox/26.0
else if(/firefox\/(\d+\.\d+)/.test(ual))
{
	Prototype.Browser.Firefox=true;
	Prototype.BrowserFeatures['Type']='Firefox';
   	Prototype.BrowserFeatures['Version']=new Number(RegExp.$1);
}
// Mercury: mozilla/5.0 (ipad; cpu os 7_0_4 like mac os x) applewebkit/537.51.1 (khtml, like gecko) mercury/8.9.4 mobile/11b554a safari/9537.53
else if(/mercury\/(\d+\.\d+)/.test(ual))
{
	Prototype.Browser.Mercury=true;
	Prototype.BrowserFeatures['Type']='Mercury';
   	Prototype.BrowserFeatures['Version']=new Number(RegExp.$1);
}
// Opera: Opera/9.80 (Linux armv6l; U; CE-HTML/1.0 NETTV/3.0.1;; en) Presto/2.6.33 Version/10.60
else if(/opera\/(\d+\.\d+)/.test(ual))
{
	Prototype.Browser.Opera=true;
	Prototype.BrowserFeatures['Type']='Opera';
   	Prototype.BrowserFeatures['Version']=new Number(RegExp.$1);
}
// Opera: Mozilla/5.0 (Windows NT 6.0; WOW64) AppleWebKit/537.36 (KHTLM, like Gecko) Chrome/31.0.1650.57 Safari/537.36 OPR/18.0.1284.68
else if(/opr\/(\d+\.\d+)/.test(ual))
{
	Prototype.Browser.Opera=true;
	Prototype.BrowserFeatures['Type']='Opera';
   	Prototype.BrowserFeatures['Version']=new Number(RegExp.$1);
}
// Opera: mozilla/5.0 (ipad; cpu os 8_2 like mac os x) applewebkit/600.1.4 (khtml, like gecko) opios/10.0.0.89592 mobile/12d508 safari/600.1.4
else if(/opios\/(\d+\.\d+)/.test(ual))
{
	Prototype.Browser.Opera=true;
	Prototype.BrowserFeatures['Type']='Opera';
   	Prototype.BrowserFeatures['Version']=new Number(RegExp.$1);
}
// Iron: Mozilla/5.0 (Windows NT 6.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Iron/30.0.1650.0 Chrome/31.0.1650.63 Safari/537.36
else if(/iron\/(\d+\.\d+)/.test(ual))
{
	Prototype.Browser.Iron=true;
	Prototype.BrowserFeatures['Type']='Iron';
   	Prototype.BrowserFeatures['Version']=new Number(RegExp.$1);
}
// Chrome: Mozilla/5.0 (Windows NT 6.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/31.0.1650.63 Safari/537.36
else if(/chrome\/(\d+\.\d+)/.test(ual))
{
	Prototype.Browser.Chrome=true;
	Prototype.BrowserFeatures['Type']='Chrome';
   	Prototype.BrowserFeatures['Version']=new Number(RegExp.$1);
}
// Chrome: mozilla/5.0 (ipad; cpu os 8_2 like mac os x) applewebkit/600.1.4 (khtml, like gecko) crios/41.0.2272.58 mobile/12d508 safari/600.1.4
else if(/crios\/(\d+\.\d+)/.test(ual))
{
	Prototype.Browser.Chrome=true;
	Prototype.BrowserFeatures['Type']='Chrome';
   	Prototype.BrowserFeatures['Version']=new Number(RegExp.$1);
}
// Safari: Mozilla/5.0 (Windows NT 6.0; WOW64) AppleWebKit/534.57.2 (KHTML, like Gecko) Version/5.1.7 Safari/534.57.2
else if(/version\/(\d+\.\d+)/.test(ual))
{
	Prototype.Browser.Safari=true;
	Prototype.BrowserFeatures['Type']='Safari';
   	Prototype.BrowserFeatures['Version']=new Number(RegExp.$1);
}

// Mobiles (iPad, Android, Tablet or Mobile)
if(ual.indexOf('ipad') > 0)
	Prototype.BrowserFeatures['Mobile']='iPad';
else if(ual.indexOf('android') > 0)
	Prototype.BrowserFeatures['Mobile']='Android';
else if(ual.indexOf('tablet') > 0)
	Prototype.BrowserFeatures['Mobile']='Tablet';
else if(ual.indexOf('mobile') > 0)
	Prototype.BrowserFeatures['Mobile']='Mobile';

// Register event handlers
document.observe('dom:loaded', fxf_eh_domLoaded);
document.observe('dblclick', fxf_eh_dblClick);
document.observe('keydown', fxf_eh_keyDown);
document.observe('keyup', fxf_eh_keyUp);
document.observe('keypressed', fxf_eh_keyPressed);
document.observe('submit', fxf_eh_submit);
document.observe('mousedown', fxf_eh_mouseDown);
document.observe('mousemove', fxf_eh_mouseMove);
document.observe('mouseover', fxf_eh_mouseOver);
document.observe('mouseout', fxf_eh_mouseOut);
document.observe('contextmenu', fxf_eh_contextMenu);
document.observe('help', fxf_eh_help);
if(Prototype.Browser.IE && (Prototype.BrowserFeatures['Version'] < 9))
{
	document.observe('focusin', fxf_eh_focus);
	document.observe('focusout', fxf_eh_blur);
}
else
{
	if(Prototype.Browser.Firefox)
	{
		document.addEventListener('focus', fxf_eh_focus, true);
		document.addEventListener('blur', fxf_eh_blur, true);
	}
	else
	{
		document.observe('focusin', fxf_eh_focus);
		document.observe('focusout', fxf_eh_blur);
	}
	document.observe('change', fxf_eh_change);
}
//alert('ual='+ual+'\n\nPrototype.Browser.IE='+Prototype.Browser.IE+'\n\nType='+Prototype.BrowserFeatures['Type']+'\nVersion='+Prototype.BrowserFeatures['Version']+'\nMobile='+Prototype.BrowserFeatures['Mobile']);

function fxf_eh_domLoaded(event)
{
	// Debug
	oDebug.fxdebug=$('fxdebug');
	oDebug.fxdebugm=$('fxdebugm');
	oDebug.fxdebugl=$('fxdebugl');
	oDebug.fxdebugb=$('fxdebugb');
	oDebug.fxdebugai=$('fxdebugai');
	if(debug)
		oDebug.fxdebug.style.display='';
	else
		oDebug.fxdebug.style.display='none';

	// Taskbar
	oID.fxtask=$('fxtask');
	oID.fxtaskp=$('fxtaskp');
	oID.fxtaskpp=$('fxtaskpp');
	oID.fxtaskt=$('fxtaskt');
	oID.fxtaski=$('fxtaski');
	oID.fxtaskc=$('fxtaskc');
	oID.fxtaskl=$('fxtaskl');
	oID.fxtasklp=$('fxtasklp');
	oID.fxtaskli=$('fxtaskli');
	oID.fxtaskli.style.display='none';
	oID.fxtaskd=$('fxtaskd');
	oID.fxtaskdp=$('fxtaskdp');
	oID.fxtasku=$('fxtasku');
	oID.fxtaskup=$('fxtaskup');
	oID.fxtaskf=$('fxtaskf');
	oID.fxtasksi=$('fxtasksi');
	oID.fxtasksh=$('fxtasksh');

	// Wait Screen
	oID.waitscreen=$('waitscreen');
	oID.waitcontent=$('waitcontent');
	if(oID.waitcontent)
		oID.waitcontent.innerHTML='';

	// Default help file
	oID.fxphoto_frame=$('fxphoto_frame');
	oID.fxphoto_frame_href=$('fxphoto_frame_href');

	// Context menu
	oID.cmenu=$('cmenu');
	oID.cmentries=$('cmentries');

	// Select popup
	oID.selpop=$('selpop');
	oID.selpop.style.display='none';
	oID.selpopsearch=$('selpopsearch');
	oID.selpopsearch.style.display='none';
	oID.selpopsearchtext=$('selpopsearchtext');
	oID.selpoptoggle=$('selpoptoggle');
	oID.selpoptoggle.style.display='none';
	oID.selpopptype=$('selpopptype');
	oID.selpopptype.style.display='none';
	oVar.selsearch=false;
	oVar.lastSelUID='';
	oVar.lastSelElement=null;
	oVar.lastMSelUID='';
	oVar.lastMSelElement=null;
	oVar.mSelElement=null;

	// Text popup
	oID.txtpop=$('txtpop');
	oID.txtpop.style.display='none';
	oID.txtpoptoggle=$('txtpoptoggle');
	oID.txtpoptoggle.style.display='none';
	oID.txtpopta=$('txtpopta');
	oVar.lastTxtUID='';
	oVar.lastTxtElement=null;

	// Last element
	oVar.lel=null;
	oVar.fxv=null;

	// Init Changed and Busy
	fxf_fn_resetChanged();

	// Favorite popup
	oID.favpop=$('favpop');

	// Search popup
	oID.searchpop=$('searchpop');

	// Table row marker
	oID.trmarkerl=$('trmarkerl');
	oID.trmarkert=$('trmarkert');
	oID.trmarkerr=$('trmarkerr');
	oID.trmarkerb=$('trmarkerb');

	// Textarea converter
	oID.taconv=$('taconv');

	// Resource PP/SP popup
	oID.hrppsppop=$('popup_res_ppsp');

	// Tooltip
	oID.tooltip=$('tooltip');			// ...Box
	oID.ttbackground=$('ttbackground');	// ...Background
	oID.ttpointer=$('ttpointer');		// ...Pointer
	oID.ttentry=$('ttentry');			// ...Text

	// Toolbar
	oID.fxtool=$('fxtool');
	oID.fxtooli=$('fxtooli');
	oID.fxtoolai=$('fxtoolai');
	oID.fxsrc=$('fxsrc');	// ...Search
	oID.fxsrc.style.display='none';
	oID.fxsrcinput=$('fxsrcinput');	// Search
	oID.fxsrcinput.style.width=tbswidth+'px';
	oID.fxsrcicon=$('fxmrcicon');	// Search icon
	oID.fxlog=$('fxlog');	// ...Logout/Login
	oID.fxlog.style.display='none';
	oID.fxinf=$('fxinf');	// ...Info
	oID.fxinf.style.display='none';
	oID.fxhlp=$('fxhlp');	// ...Help
	oID.fxhlp.style.display='none';
	oID.fxset=$('fxset');	// ...Settings
	oID.fxset.style.display='none';
	oID.fxflt=$('fxflt');	// ...Filter
	oID.fxflt.style.display='none';
	oID.fxfltsel=$('fxfltsel');
	oID.fxfltsel.style.width=tbswidth+'px';
	oID.fxfav=$('fxfav');	// ...Favorite
	oID.fxfav.style.display='none';
	oID.fxbak=$('fxbak');	// ...Back
	oID.fxbak.style.display='none';
	oID.fxhom=$('fxhom');	// ...Home
	oID.fxhom.style.display='none';

	// Main menu
	oID.fxmenu1=$('fxmenu1');
	oID.fxmenu1f=$('fxmenu1f');
	oID.fxmenu1c=$('fxmenu1c');
	oID.fxmenu1l=$('fxmenu1l');
	oID.fxmenu1r=$('fxmenu1r');
	oID.fxmenu2=$('fxmenu2');
	oID.fxmenu2f=$('fxmenu2f');
	oID.fxmenu2c=$('fxmenu2c');
	oID.fxmenu2l=$('fxmenu2l');
	oID.fxmenu2r=$('fxmenu2r');
	oID.fxmenu3=$('fxmenu3');
	oID.fxmenu3f=$('fxmenu3f');
	oID.fxmenu3c=$('fxmenu3c');
	oID.fxmenu3l=$('fxmenu3l');
	oID.fxmenu3r=$('fxmenu3r');

	// Main area
	oID.fxmain=$('fxmain');

	// Work area
	oID.fxframe=$('fxframe');
	oID.fxpath=$('fxpath');
	oID.fxtaction=$('fxtaction');
	oID.fxworkframe=$('fxworkframe');
	oID.fxworkf=$('fxworkf');
	oID.fxwork=$('fxwork');

	// Buttons
	oID.fxbuttons=$('fxbuttons');

	// Header + Footer
	oID.fxheaderc=$('fxheaderc');
	oID.fxheader=$('fxheader');
	oID.fxfooter=$('fxfooter');

	// Message
	oID.imessage=$('imessage');
	oID.imessagem=$('imessagem');
	oID.imessageai=$('imessageai');
	oID.imessage.style.display='none';

	// Workflow
	oID.iworkflow=$('iworkflow');
	oID.iworkflowi=$('iworkflowi');
	oID.iworkflowa=$('iworkflowa');
	oID.iworkflowai=$('iworkflowai');
	oID.iworkflowat=$('iworkflowat');
	oID.iworkflow.style.display='';
	oVar.dimiworkflowat=fxf_fn_getBCR(oID.iworkflowat);
	oID.iworkflow.style.display='none';

	// Projects
	oID.iprojects=$('iprojects');
	oID.iprojectsm=$('iprojectsm');
	oID.iprojectsa=$('iprojectsa');
	oID.iprojectsai=$('iprojectsai');
	oID.iprojectsat=$('iprojectsat');
	oID.iprojects.style.display='';
	oVar.dimiprojectsat=fxf_fn_getBCR(oID.iprojectsat);
	oID.iprojects.style.display='none';

	// Popup Interactions
	oID.iact=$('iact');
	oID.iacont=$('iacont');
	oID.iaentries=$('iaentries');
	oID.iatitle=$('iatitle');
	oID.iabox=$('iabox');
	oID.iainfo=$('iainfo');

	// Popup Transactions
	oID.ptrans=$('ptrans');
	oID.ptcont=$('ptcont');
	oID.pttitle=$('pttitle');
	oID.ptbuttons=$('ptbuttons');
	oID.ptmain=$('ptmain');
	oID.ptfooter=$('ptfooter');

	// Popup Message
	oID.ptmessage=$('ptmessage');
	oID.ptmessagem=$('ptmessagem');
	oID.ptmessageai=$('ptmessageai');
	oID.ptmessage.style.display='none';

	// Uploader
	oID.oUploader={
		'uploader':$('uploader'),
		'uploader_form':$('uploader_form'),
		'uploader_header_text':$('uploader_header_text'),
		'uploader_select_text':$('uploader_select_text'),
		'uploader_select':$('uploader_select'),
		'uploader_zone_text':$('uploader_zone_text'),
		'uploader_zone':$('uploader_zone'),
		'uploader_iframe':$('uploader_iframe'),
		'uploaderh_frm':$('uploaderh_frm'),
		'uploaderh_lts':$('uploaderh_lts'),
		'uploaderh_upn':$('uploaderh_upn'),
		'uploaderh_div':$('uploaderh_div'),
		'multi':0,
		'filelist':[],
		'totalsize':0
	};
	oID.oUploader.uploader_zone.observe('dragstart', fxf_eh_dragStart);
	oID.oUploader.uploader_zone.observe('dragenter', fxf_eh_dragEnter);
	oID.oUploader.uploader_zone.observe('dragover', fxf_eh_dragOver);
	oID.oUploader.uploader_zone.observe('dragleave', fxf_eh_dragLeave);
	oID.oUploader.uploader_zone.observe('dragend', fxf_eh_dragEnd);
	oID.oUploader.uploader_zone.observe('drop', fxf_eh_drop);

	// Initialize data
	var idata=$('idata').innerHTML.split('|');
	oFXP.lcnt=0;					// Load counter
	oFXP.tr=0;						// Login
	oFXP.ltr=0;						// Last Transaction
	oFXP.ptr=0;						// Popup Transaction
	oFXP.action=0;					// Action
	oFXP.mstructure=[];				// Menu structure
	oFXP.mentries=0;				// Menu main entries
	oFXP.mnid='';					// Last menu id
	oFXP.locseskey=idata[0];		// Encoded location + session storage id
	oFXP.lts=idata[1];				// Login timestamp
	oFXP.lang=parseInt(idata[2]);	// Language: 1=Deutsch, 7=English
	oFXP.design=idata[3];			// Design: 000default (= default)
	oFXP.zoom=parseInt(idata[4]);	// Zoom: Years[0=1px], Months[1=2px, 2=4px], Weeks[3=6px, 4=8px], Days[5=12px, 6=16px, 7=32px]
	oFXP.pwindow=null;				// Preview window
	oFXP.field=null;				// Variable field
//alert('Initial data:\n\ntr='+oFXP.tr+', locseskey='+oFXP.locseskey+', lts='+oFXP.lts+', lang='+oFXP.lang+', design='+oFXP.design+', zoom='+oFXP.zoom);
	fxf_fn_taskMsg('+ - Vars initialized');

	var dbgl=fxf_fn_getSelectedValue($('dbgmode'));
	gSet.dbgl=parseInt(dbgl.value);

	fxf_fn_writeDebug('log+', '<b>userAgent</b>:<br />'+navigator.userAgent);
	if(Prototype.Browser.IE)
		fxf_fn_writeDebug('log+', '- <b>IE</b>, Version '+Prototype.BrowserFeatures['Version']);
	else if(Prototype.Browser.Firefox)
		fxf_fn_writeDebug('log+', '- <b>Firefox</b>, Version '+Prototype.BrowserFeatures['Version']);
	else if(Prototype.Browser.Mercury)
		fxf_fn_writeDebug('log+', '- <b>Mercury</b>, Version '+Prototype.BrowserFeatures['Version']);
	else if(Prototype.Browser.Opera)
		fxf_fn_writeDebug('log+', '- <b>Opera</b>, Version '+Prototype.BrowserFeatures['Version']);
	else if(Prototype.Browser.Iron)
		fxf_fn_writeDebug('log+', '- <b>SRWare Iron</b>, Version '+Prototype.BrowserFeatures['Version']);
	else if(Prototype.Browser.Chrome)
		fxf_fn_writeDebug('log+', '- <b>Google Chrome</b>, Version '+Prototype.BrowserFeatures['Version']);
	else if(Prototype.Browser.Safari)
		fxf_fn_writeDebug('log+', '- <b>Safari</b>, Version '+Prototype.BrowserFeatures['Version']);
	if(Prototype.Browser.WebKit)
		fxf_fn_writeDebug('log+', '- <b>WebKit</b>');
	if(Prototype.Browser.Gecko)
		fxf_fn_writeDebug('log+', '- <b>Gecko</b>');
	if(Prototype.BrowserFeatures['Mobile'].length)
		fxf_fn_writeDebug('log+', '- <b>'+Prototype.BrowserFeatures['Mobile']+'</b>');

	fxf_fn_taskMsg('+ - Browser detected: '+Prototype.BrowserFeatures['Type']+' (Version '+Prototype.BrowserFeatures['Version']+')');
	if(Prototype.BrowserFeatures['Mobile'].length)
		fxf_fn_taskMsg(' / '+Prototype.BrowserFeatures['Mobile']);
//alert('Browser: '+Prototype.BrowserFeatures['Type']+' (Version '+Prototype.BrowserFeatures['Version']+') / '+Prototype.BrowserFeatures['Mobile']);

	// Get dimensions
	fxf_fn_getDimensions();

	// Adjust logo
	var fxlogoi=$('fxlogoi');
	if(fxlogoi)
	{
		if((dim.sd.pwidth < 500) || (dim.sd.pheight < 557))
		{
			var lif=Math.min(dim.sd.pwidth/560, dim.sd.pheight/617);
			var liw=Math.floor(560*lif);
			var lih=Math.floor(617*lif);
			fxlogoi.style.left=-(Math.floor(liw/2))+'px';
			fxlogoi.style.top=-(Math.floor(lih/2))+'px';
			fxlogoi.style.width=liw+'px';
			fxlogoi.style.height=lih+'px';
		}
		$('fxlogo').style.display='';
	}

	fxf_eh_checkPatch(0);
}

function fxf_eh_checkPatch(pcnt)
{
	if(pcnt > 60)	// Always start after 2 min
		fxf_eh_startTR(pcnt);

	var url=fxf_fn_gProgram('check_patch', 'pt=1&pcnt='+pcnt+fxf_fn_gParam());
//alert('url='+url);

	new Ajax.Request(url,
	{
		method:'get', asynchronous:asajax,
		onSuccess: function(transport)
		{
			var pr=parseInt(transport.responseText);
			if(pr)
			{
				if(!pcnt)
					fxf_fn_waitScreen('patching');
				pcnt++;
				if(pr == 1)
					fxf_eh_startTR(0);
				else
					fxf_eh_checkPatch.delay(1.0, pcnt);
			}
			else
				fxf_eh_startTR(pcnt);
		},
		onFailure: function()
		{
			fxf_eh_startTR(pcnt);
		}
	});
}

function fxf_eh_startTR(pcnt)
{
	if(pcnt)
		fxf_fn_waitScreen('');

	// Update clock
	fxf_fn_clock(true,true);

	// Load transaction
	fxf_fn_loadTR(0,'');
}

function fxf_eh_dblClick(event)
{
	cmd='D';
	fxf_fn_setPageStatus('red');

	var element=fxf_fn_getField($(Event.element(event)),false);
	if(!element || !element.id)
		return;
//fxf_fn_taskMsg('fxf_eh_dblClick: element.id='+element.id);

	if((oID.searchpop.style.display == 'none') && (oID.selpop.style.display == 'none') && (oID.txtpop.style.display == 'none'))
	{
		if((oFXP.tr == 189) && oID.sdg_container && (oID.iainfo.style.display == 'none'))
		{
			fxf_fn_dispMenu(event,-1);
			oID.saction.style.display='none';
			oID.gaction.style.display='none';
			soi='';
			soii=-1;
			moi='';
			moii=-1;

			var ei5=element.id.substr(0,5);

			if((ei5 == 'ntuid') || (ei5 == 'stuid') || (ei5 == 'giuid') || (ei5 == 'nsuid') || (ei5 == 'nmuid'))
			{
				var pid=element.id.substr(2);
				var pc=fxf_fn_getKeyFromID('P', pid);
				if(fxf_fn_checkRename(pc))
					fxf_fn_editText(element);
			}
			else if(ei5 == 'enuid')
			{
				var pid=element.id.substr(2);
				var pc=fxf_fn_getKeyFromID('P', pid);
				if(fxf_fn_checkRenumber(pc))
					fxf_fn_editNo(element);
			}
			else
			{
//alert('fxf_eh_dblClick('+element.id+');');
			}

			Event.stop(event);
		}
	}
}

function fxf_eh_contextMenu(event,selement)
{
	cmd='R';

	if(selement)
		var element=selement;
	else
	{
		var element=fxf_fn_getField($(Event.element(event)),false);
		var eup=null;
		while(element && !element.id && !element.onclick)
		{
			eup=element.up();
			if(!eup)
				break;
			element=fxf_fn_getField(eup,false);
		}
	}

	if(!element || !element.id)
		return true;
//fxf_fn_taskMsg('fxf_eh_contextMenu: element.id='+element.id);

	// Project action menu
	if(element.id.substr(0,6) == 'pjsmp_')
	{
		fxf_fn_dispMenu(event,-1);
		fxf_fn_openPJSMMenu(event,element.id.substr(6));

		Event.stop(event);
	}
	// Time-registration actions
	else if((oFXP.tr == 83) && oID.trw_container)
	{
		fxf_fn_dispMenu(event,-1);

		var om=0;
		var dr='';
		var tp='st';
		if(element.id.substr(0,4) == 'dko_')
		{
			om=4;
			dr=element.id.substr(4);
			element.style.display='none';
		}
		else if(element.id.substr(0,6) == 'trw_e_')
		{
			om=6;
			dr=element.id.substr(6,element.id.length-9);
			tp=element.id.substr(element.id.length-2,2);
		}
		if(om)
		{
			var dt=dr.substr(0,8);
			var rw=dr.substr(9);
			fxf_fn_trwContextMenu(event,dt,rw,tp);
			Event.stop(event);
		}
	}
	// DMS actions
	else if(((oFXP.tr == 107) || (oFXP.tr == 110)) && tSet.dms && tSet.dms.structure)
	{
		fxf_fn_dispMenu(event,-1);

		// Valid id's:
		// ...swork, d_fname[*], imain	(= empty line in structure, doclist and info)
		// ...dms_dirn_*				(= dirname in structure)
		// ...nup_cb[*]					(= filename checkbox)
		// ...fname[*]					(= dirname or filename in doclist)
		// ...dw_fname, dw_auto, ...	(= empty column line in doclist)
		var om=0;
		var tp='';
		var id=0;
		var i=element.id.indexOf('[');
		if(i > 0)
		{
			tp=element.id.substr(0,i);
			if(tp.substr(0,2) == 'd_')
				tp=tp.substr(2);
			id=parseInt(element.id.substr(i+1));
//alert('RMB (id='+element.id+') -> tp='+tp+', id='+id);
			if(!tSet.dms.save.length && (tSet.dms.rights[0] == tSet.dms.rights[1]))
			{
				var cb=$('nup_cb['+id+']');
				if(cb)
				{
					if(!cb.checked)
					{
						cb.checked=true;
						fxf_fn_dmsSelectFile(id, false);
					}
					om=1;	// File
				}
				else
					om=2;	// Directory
			}
		}
		else if(element.id.substr(0,9) == 'dms_dirn_')
		{
			tp=element.id.substr(0,8);
			id=parseInt(element.id.substr(9));
			om=2;	// Directory
		}
		else if((element.id == 'swork') || (element.id.substr(0,3) == 'dw_'))
		{
			tp=element.id.substr(0,1);	// 's' or 'd'
			om=3;	// Empty line
		}
		else
		{
//alert('RMB (id='+element.id+')');
		}

		if(om)
		{
			fxf_fn_dmsContextMenu(event,om,tp,id);
			Event.stop(event);
		}
	}
	// Matrix actions
	else if((oFXP.tr == 189) && oID.sdg_container && (oID.iainfo.style.display == 'none'))
	{
		fxf_fn_dispMenu(event,-1);
		oID.saction.style.display='none';
		oID.gaction.style.display='none';
		var scm=false;
		soi='';
		soii=-1;
		moi='';
		moii=-1;

		// Project
		var ei3=element.id.substr(0,3);
		var ei5=element.id.substr(0,5);
		if((ei5 == 'gsuid') || (ei5 == 'gbuid') || (ei5 == 'geuid') || (ei5 == 'gnuid') || (ei5 == 'nduid') || (ei5 == 'nnuid') || (ei5 == 'ntuid') || (ei5 == 'guuid'))	// Project (Start node, bar or end node)
		{
			var pid=element.id.substr(2);
			pAct=fxf_fn_getKeyFromID('P', pid);
			if(pArray[pAct].rights & 8)
			{
				dAct=-1;
				fxf_fn_showContextMenu(event);
				scm=true;
				mka='G';
			}
			else
				pAct=-1;
		}
		else if((ei5 == 'giuid') || (ei5 == 'nsuid') || (ei5 == 'nmuid'))	// Milestone
		{
			var pid=element.id.substr(2);
			pAct=fxf_fn_getKeyFromID('P', pid);
			if(pArray[pAct].rights & 8)
			{
				dAct=-1;
				fxf_fn_showContextMenu(event);
				scm=true;
				mka='G';
			}
			else
				pAct=-1;
		}
		else if(element.id.substr(0,2) == 'dp')	// Dependency
		{
			var did=element.id.substr(3);
			pAct=-1;
			dAct=fxf_fn_getKeyFromID('D', did);
			fxf_fn_showContextMenu(event);
			scm=true;
			mka='G';
		}
		else if(((ei5 == 'gbars') || (ei5 == 'pfram') || (ei5 == 'glmar') || (ei5 == 'gcmar')) && !pArray.length) // Main project
		{
			pAct=-1;
			dAct=-1;
			fxf_fn_showContextMenu(event);
			scm=true;
			mka='G';
		}
		else
		{
//alert('fxf_eh_contextMenu('+element.id+');');
		}

		if(scm)
			Event.stop(event);
	}
}

function fxf_eh_submit(event)
{
	fxf_fn_setPageStatus('red');
//alert('Submit event: target='+event.target+', target.id='+event.target.id+', target.name='+event.target.name+', type='+event.type);

	if(event.target && event.target.id && (event.target.id == 'fxheaderform'))
	{
		Event.stop(event);
		return false;
	}
}

function fxf_eh_mouseDown(event,selement)
{
	cmd='M';
	fxf_fn_setPageStatus('red');

	var rmb=false;
	if(event.button)
		rmb=(event.button == 2);
	else if(event.which)
		rmb=(event.which == 3);
	if(rmb)
	{
		fxf_fn_dispMenu(event,0);
		Event.stop(event);
		return false;
	}

	if(selement)
		var element=selement;
	else
	{
		var element=fxf_fn_getField($(Event.element(event)),false);
		var eup=null;
		while(element && !element.id && !element.onclick)
		{
			eup=element.up();
			if(!eup)
				break;
			element=fxf_fn_getField(eup,false);
		}
	}
//fxf_fn_taskMsg('fxf_eh_mouseDown: element.id='+element.id+', element.onclick='+element.onclick+' -- keys: shift='+event.shiftKey+', ctrl='+event.ctrlKey+', alt='+event.altKey);
//fxf_fn_writeDebug('log+', 'fxf_eh_mouseDown: element.id='+element.id+' -- keys: shift='+event.shiftKey+', ctrl='+event.ctrlKey+', alt='+event.altKey);

	// Onclick?
	if(element && element.onclick)
		return;

	fxf_fn_dispMenu(event,0);

	// Text popup?
	if(element && element.id && (element.id.substr(0,6) != 'txtpop') && oVar.lastTxtElement && (oID.txtpop.style.display != 'none'))
		fxf_fn_textSet(oVar.lastTxtElement);

	// Close menu
	if(oFXP.mnid.length && (element.id.substr(0,6) != 'fxmenu') && (element.id.substr(0,4) != 'fxm_'))
		fxf_fn_drawMenu('');
	// Handle text popup enlarge
	else if(element && element.id && (element.id.substr(0,12) == 'txtpoptoggle') && oVar.lastTxtElement)
	{
		fxf_fn_textPopup(1,oVar.lastTxtElement);
		Event.stop(event);
		return false;
	}
	// Handle select field search
	else if(element && element.id && ((element.id.substr(0,12) == 'selpopsearch') || (element.id.substr(0,11) == 'selpopptype')))
	{
		if(element.id == 'selpopsearchtext')
		{
			element.focus();
			element.select();
			selsc++;
		}
		else if(element.id.substr(0,11) == 'selpopptype')
			fxf_fn_searchSelPopup.delay(oFXP.sdelay, 0);
		else
		{
			fxf_fn_searchSelPopup(13);
			oVar.selsearch=true;
		}
		Event.stop(event);
		return false;
	}
	// Handle multiselect toggle
	else if(element && element.id && (element.id.substr(0,12) == 'selpoptoggle'))
	{
		if((mselinfo.mode > 1) && mselinfo.element)
		{
			fxf_fn_multiselToggle(-1,oVar.mSelElement);
			fxf_fn_multiselToggle(1,oVar.mSelElement);
		}
		else if(oVar.mSelElement)
		{
			fxf_fn_multiselToggle(0,oVar.mSelElement);
		}
		Event.stop(event);
		return false;
	}
	// Handle multiselect checkbox
	else if(element && element.id && (element.id.substr(0,9) == 'mselentry'))
	{
		var mc=parseInt(element.id.substr(11));
		var mt=element.id.substr(9,1);
		var cn=mselinfo.entries[mc-1].className;
		var cbe=$('mselentryc_'+mc);
//alert('mc='+mc+', mt='+mt+', cn='+cn);
		if(cbe)
		{
			if(cn == 'msentrysel')
			{
				if(mt != 'c')
					cbe.checked=false;
				$('mselentryl_'+mc).className='msentry';
				mselinfo.entries[mc-1].className='msentry';
				var cc=0;
				for(var m=0; m<mselinfo.entries.length; m++)
				{
					if(mselinfo.entries[m].className == 'msentrysel')
						cc++;
				}
				if(!cc)
				{
					$('mselentryl_1').className='msentrysel';
					$('mselentryc_1').checked=true;
					mselinfo.entries[0].className='msentrysel';
				}
			}
			else
			{
				if(mt != 'c')
					cbe.checked=true;
				$('mselentryl_'+mc).className='msentrysel';
				mselinfo.entries[mc-1].className='msentrysel';
				if(oFXP.tr != 83)
				{
					if(mc == 1)
					{
						for(var m=1; m<mselinfo.entries.length; m++)
						{
							var msc=$('mselentryc_'+(m+1));
							if(msc)
							{
								$('mselentryl_'+(m+1)).className='msentry';
								msc.checked=false;
							}
							mselinfo.entries[m].className='msentry';
						}
					}
					else
					{
						$('mselentryl_1').className='msentry';
						$('mselentryc_1').checked=false;
						mselinfo.entries[0].className='msentry';
					}
				}
			}

			mselinfo.element.fxv='';
			for(var m=0; m<mselinfo.entries.length; m++)
			{
				if(mselinfo.entries[m].className == 'msentrysel')
				{
					if(mselinfo.element.fxv.length)
						mselinfo.element.fxv += '|';
					mselinfo.element.fxv += mselinfo.entries[m].attributes['svalue'].value;
				}
			}
//alert('fxv='+mselinfo.element.fxv);
			mselinfo.element.attributes['svalue'].value=mselinfo.element.fxv;
			oVar.lastMSelUID=mselinfo.entries[mc-1].id;
			oFXP.addchg=true;
			fxf_fn_saveElement.delay(oFXP.sdelay, mselinfo.element);
		}
		Event.stop(event);
		return false;
	}
	// Handle multiselect container
	else if(element && element.className && (element.className == 'fxfsm'))
	{
		fxf_fn_selPopup(false);
//		fxf_fn_multiselToggle(0,element);
	}
	else
	{
		// Handle click on red marked text in new selected option
		if(element && element.uid && (element.uid.substr(0,4) == 'fsps'))
			element=fxf_fn_getField(element.up(),false);
		if(element && element.uid && (oVar.lastSelUID != element.uid) && (element.uid != 'selpop'))
		{
			// Handle new selected option
			if(element.uid.substr(0,4) == 'fspo')
			{
				if(element.attributes && element.attributes['svalue'])
				{
					var cn3=fxf_fn_getSelType(oVar.lastSelElement);
//alert('Option selected\n\nuid='+element.uid+'\nclassName='+element.className+', last className='+oVar.lastSelElement.className+'\nsvalue='+element.attributes['svalue'].value+'\n\n--- oVar.lastSelElement ---\n\n.outerHTML='+oVar.lastSelElement.outerHTML+'\n\n.innerHTML='+oVar.lastSelElement.innerHTML+'\n\n--- element ---\n\n.outerHTML='+element.outerHTML+'\n\n.innerHTML='+element.innerHTML+'\n\n---\n\ncn3='+cn3);
					oVar.lastSelElement.innerHTML=fxf_fn_convertHTMLEntities(element.innerHTML,true)+'<div class="'+cn3+'slc" style="cursor:pointer;"><span class="'+cn3+'sli"></span></div>';
					oVar.lastSelElement.attributes['svalue'].value=element.attributes['svalue'].value;
					oVar.lastSelElement.fxv=element.attributes['svalue'].value;
					var sa=['background','color','fontStyle','fontWeight','textDecoration'];
					for(var s=0; s<sa.length; s++)
					{
						if(element.style[sa[s]])
							oVar.lastSelElement.style[sa[s]]=element.style[sa[s]];
						else if(oVar.lastSelElement.style[sa[s]])
							oVar.lastSelElement.style[sa[s]]='';
					}
					oFXP.addchg=true;
					if(oFXP.tr == 110)
						fxf_fn_dmsSave('change', oVar.lastSelElement.id);
					else
						fxf_fn_saveElement(oVar.lastSelElement);
					fxf_fn_unfocus(true,false,oVar.lastSelElement);
				}
				Event.stop(event);
				return false;
			}
			if(oVar.lastSelElement)
				fxf_fn_unfocus(true,false,null);
			if((element.uid.substr(0,5) != 'fspmo') && oVar.lastMSelElement && (oVar.lastMSelElement.id != element.uid))
				fxf_fn_unfocus(false,true,null);
		}
		if(!element || !element.uid || !element.fxf || (element.fxf == 'us'))
		{
			if(!element.uid || (element.uid != 'selpop'))
				fxf_fn_unfocus(true,true,null);
			if((!element.uid || (element.uid.substr(0,5) != 'fxfav')) && (oID.favpop.style.display != 'none'))
			{
				fxf_fn_favPopup(false);
			}
			if((!element.uid || ((element.uid.substr(0,5) != 'fxsrc') && (element.uid.substr(0,9) != 'searchpop'))) && (oID.searchpop.style.display != 'none'))
			{
				fxf_fn_searchPopup(false);
			}
			if(element.fxf)
				return false;
		}
	}

//fxf_fn_writeDebug('log+', '<b>fxf_eh_mouseDown</b>:<br />id='+element.id+', name='+element.name+', type='+element.type+'<br />-&gt;uid='+element.uid+', fxf='+element.fxf);
	if(element.fxf == 'bs') // Submit button
	{
//alert('Submit button: element.uid='+element.uid);
		fxf_fn_fxSubmitButton(element);
		if(element.uid == 'Button_Vorschau')
		{
			Event.stop(event);
			return false;
		}
	}
	else if(element.fxf == 'br') // Reset button
		fxf_fn_fxResetButton(element);
	else if(element.fxf == 'is') // Submit image
		fxf_fn_fxSubmitImage(element);
	else if(element.fxf == 'il') // Link image
	{
		// Click on date popup icon
		if(element.hasClassName('date_pop'))
		{
			if(fxf_fn_checkDisabled(element))
				return;
			fxf_fn_openDatePopUp(element.previous(), event);
		}
		else
		{
			var wtxt='loading';
			fxf_fn_waitScreen(wtxt);
//alert('Link image: '+element.attributes['href'].value);
			fxf_fn_fxLink.delay(oFXP.sdelay, element);
		}
	}
	else if(element.attributes && element.attributes['ud'])	// UndoDisabled
		fxf_fn_askUndoDisable(element);
	else if(element.fxf == 'sl') // Select
	{
		if(oVar.lastSelUID == element.uid)
			fxf_fn_selPopup(false);
		else
		{
//fxf_fn_writeDebug('log+', '<b>selected</b>: '+element.attributes['svalue'].value);
			fxf_fn_unfocus(true,true,null);
			if(element.className.substr(element.className.length-1,1) != 'g')
				fxf_fn_selPopup(element);
		}
	}
	else if(element.uid.substr(0,5) == 'fspmo')	// Multiselect
	{
		var msl=element.up().childElements();
		var mse=fxf_fn_getField(element.up().up(),false);
//		fxf_fn_multiselToggle(0,mse);

		mse.fxv='';
//alert('msl: msl.length='+msl.length+'\nmse: mse.uid='+mse.uid+', mse.className='+mse.className);
		if(msl && msl.length)
		{
			fxf_fn_multiselToggle(1,mse);

			var as=0;
			var fs=-1;
			var ls=-1;
			for(var m=0;m<msl.length;m++)
			{
				if(element.uid == msl[m].id)
					as=m;
				if((element.uid == msl[m].id) || ((msl[m].className == 'msentrysel') && (!oVar.lastMSelUID.length || (oVar.lastMSelUID == msl[m].id))))
				{
					if(fs < 0)
						fs=m;
					ls=m;
				}
			}
//alert('oVar.lastMSelUID='+oVar.lastMSelUID+', as='+as+', fs='+fs+', ls='+ls);

			if(msl[as].className != 'sdisabled')
			{
				for(m=0;m<msl.length;m++)
				{
					if(msl[m].className != 'sdisabled')
					{
						if(!as && (oFXP.tr != 83))
						{
							if(!m)
								msl[m].className='msentrysel';
							else
								msl[m].className='msentry';
						}
						else if(!m && (oFXP.tr != 83))
							msl[m].className='msentry';
						else if(event.shiftKey)
						{
							if((m < fs) || (m > ls))
								msl[m].className='msentry';
							else
								msl[m].className='msentrysel';
						}
						else if(!event.ctrlKey)
						{
							if(element.uid != msl[m].id)
								msl[m].className='msentry';
							else
								msl[m].className='msentrysel';
						}
						else if(element.uid == msl[m].id)
						{
							if(msl[m].className == 'msentrysel')
								msl[m].className='msentry';
							else
								msl[m].className='msentrysel';
						}
					}

					if(msl[m].className == 'msentrysel')
					{
//alert(m+': id='+msl[m].id+', className='+msl[m].className+', value='+msl[m].attributes['svalue'].value+', innerHTML='+msl[m].innerHTML);
						if(mse.fxv.length)
							mse.fxv += '|';
						mse.fxv += msl[m].attributes['svalue'].value;
					}
				}
			}
		}
		if(mse.fxv.length)
		{
			mse.attributes['svalue'].value=mse.fxv;
			oFXP.addchg=true;
			fxf_fn_saveElement(mse);
		}

		oVar.lastMSelUID=element.uid;
		oVar.lastMSelElement=mse;
		if(mse.className.substr(0,5) != 'focus')
			mse.className='focus'+mse.className;
		Event.stop(event);
	}
	else
	{
		var ei=element.id;
		var ei2=ei.substr(0,2);
		var ei3=ei.substr(0,3);
		var ei4=ei.substr(0,4);
		var ei5=ei.substr(0,5);
		var ei6=ei.substr(0,6);
		var ei7=ei.substr(0,7);
		var ei8=ei.substr(0,8);
		var ei9=ei.substr(0,9);
		var ei10=ei.substr(0,10);

		if((oID.selpoptoggle.style.display != 'none') && (ei != 'selpop') && (!element.className || (element.className != 'fxfsm')))
			fxf_fn_selPopup(false);
//alert('ei='+ei+', element.className='+element.className);

		if((ei5 == 'edit_') || (ei6 == 'nedit_'))
			return false;
//fxf_fn_writeDebug('log+', 'fxf_eh_mouseDown: '+ei);

		// Close Popup
		if(ei == 'iact')
		{
			if(oID.iainfo.style.display != 'none')
			{
				var pice=$('pop_ic_close_excl');
				if(!pice || (oFXP.tr == 107) || (oFXP.tr == 110))
					fxf_fn_fxLinkClose();
			}
		}

		// Click on "No entries" info
		else if((ei5 == 'mted_') || (ei5 == 'mtei_') || (ei5 == 'mtef_'))
		{
			var mn=parseInt(ei.substr(5));
			fxf_fn_toggleEmptyInfo(mn);
		}

		// Click on "Show/Hide" fields
		else if(ei == 'shfields')
		{
			fxf_fn_toggleFields(element,-1);
		}

		// Click on checkbox popup icon
		else if(ei5 == 'icbpu')
			fxf_fn_checkboxPopup(element, event);

		// Click on column select popup icon
		else if(ei7 == 'icrcpu_')
			fxf_fn_columnsPopup(element, event);

		// Click on column select: reset, save or delete icon
		else if(((ei6 == 'rcres_') || (ei6 == 'rcsav_') || (ei6 == 'rcdel_')) && (element.getOpacity() > 0.50))
		{
			var mn=ei.substr(6);
			if(ei6 == 'rcres_')
				fxf_fn_rcAction(mn,0,false);
			else
			{
				var url=fxf_fn_gProgram('popup_col', 'mn='+mn+fxf_fn_gParam());
				var sa=fxf_fn_getSelectedValue($('rcsel_'+mn));
				var te=$('rctxt_'+mn);
				var pst='action='+ei.substr(2,3)+'&rcsel='+sa.value+'&rctxt='+fxf_fn_escapeText(te.value,false);
//alert('url='+url+'\npst='+pst);

				new Ajax.Request(url,
				{
					method:'post', postBody:pst, asynchronous:true,
					onSuccess: function(transport)
					{
//alert('OK\n\n'+transport.responseText);
						if(transport.responseText == 'OK')
						{
							var as=sa.value.split('_');
							var nsv=as[1];
							if(ei6 == 'rcdel_')
								te.value=sa.text;
							fxf_fn_rcSet(mn,nsv);
						}
					}
				});
			}
		}

		// Enlarge/Shrink
		else if(ei5 == 'esenl')
			fxf_fn_enlarge(true);
		else if(ei5 == 'esshr')
			fxf_fn_enlarge(false);

		// Take Mask Screenshot
		else if((ei7 == 'fxphoto') && (ei.substr(ei.length-5,5) != '_disp'))
			fxf_fn_takeMaskScreenshot(ei.substr(8));

		// Home = Control Center
		else if(ei5 == 'fxhom')
		{
			fxf_fn_waitScreen('loading');
			fxf_fn_loadTR(14,'newtr');
		}

		// Back
		else if(ei5 == 'fxbak')
		{
			fxf_fn_waitScreen('reloading');
			fxf_fn_loadTR(oFXP.ltr,'newtr&back=1');
		}

		// Toggle Favorites Popup
		else if(ei5 == 'fxfav')
		{
			if(oID.favpop.style.display == 'none')
				fxf_fn_favPopup(element);
			else
				fxf_fn_favPopup(false);
		}

		// Toggle filter on/off
		else if(ei7 == 'fxflt01')
		{
			if(gSet.filter)
				gSet.filter=0;
			else
				gSet.filter=1;
			fxf_fn_setFilter();
			fxf_fn_filterChanged();
		}
		// Filter
		else if(ei5 == 'fxflt')
			fxf_fn_loadPTR(223,'newptr');

		// Toggle Settings Popup
		else if(ei5 == 'fxset')
			fxf_fn_setPopup(true);

		// Toggle Help Popup
		else if(ei5 == 'fxhlp')
			fxf_fn_hlpPopup(true);

		// Info
		else if(ei5 == 'fxinf')
		{
			fxf_fn_waitScreen('loading');
			fxf_fn_loadTR(158,'newtr');
		}

		// Search
		else if(ei5 == 'fxsrc')
		{
			if((oID.searchpop.style.display == 'none') || (ei == 'fxsrcicon'))
				fxf_fn_searchPopup(element);
		}

		// Toogle screenshot mode
		else if(ei10 == 'fxscrshtmd')
		{
			fxf_fn_toggleScreenshotMode(0);
		}

		// Toogle debug
		else if(ei8 == 'fxdebuga')
		{
			var sl=parseInt(oDebug.fxdebug.style.left);
			var nl=0;
			if(sl >= 0)
			{
				nl=-(parseInt(oDebug.fxdebug.style.width)-3);
				oDebug.fxdebugai.style.backgroundPosition='-36px 0';
			}
			else
				oDebug.fxdebugai.style.backgroundPosition='-12px 0';
			new Effect.Morph(oDebug.fxdebug, {style:'left:'+nl+'px', duration: 0.2});
		}

		// Toogle toolbar
		else if(ei7 == 'fxtoola')
		{
			var st=parseInt(oID.fxtool.style.top);
			var ny=0;
			if(st >= 0)
			{
				ny=-(parseInt(oID.fxtool.style.height)-1);
				oID.fxtoolai.style.backgroundPosition='0 0';
			}
			else
				oID.fxtoolai.style.backgroundPosition='-24px 0';
			new Effect.Morph(oID.fxtool, {style:'top:'+ny+'px', duration: 0.2});
			new Effect.Morph(oID.fxmain, {style:'top:'+(ny+98)+'px', duration: 0.2});
			gSet.maintop=(ny+98)+'px';
		}

		// Toogle message
		else if((ei9 == 'imessagea') || (ei10 == 'ptmessagea'))
		{
			if(ei9 == 'imessagea')
				var sr=parseInt(oID.imessage.style.right);
			else
				var sr=parseInt(oID.ptmessage.style.right);
			if(sr >= 0)
				fxf_fn_shrinkMessage(0.2,true);
			else
				fxf_fn_growMessage(0.2);
		}

		// Toogle workflow
		else if(ei10 == 'iworkflowa')
		{
			var sl=parseInt(oID.iworkflow.style.left);
			if(sl >= 0)
				fxf_fn_shrinkWorkflow(0.2,-1);
			else
				fxf_fn_growWorkflow(0.2);
		}
		else if((ei9 == 'pwaitscre') && (gSet.wfsl > 0))
			fxf_fn_shrinkWorkflow(0.2,-1);
		// Click on workflow settings
		else if((ei3 == 'wfb') && (gSet.wfsl > 0))
		{
			var n2=ei.substr(3,2);
			if(n2 == 'am')
				fxf_fn_changeWFSetting('wfairow',true);
			else if(n2 == 'cp')
				fxf_fn_changeWFSetting('wfcolor',true);
			else if(n2 == 'bt')
				fxf_fn_drawWFMenu(gSet.wfradius,true);
		}
		// Toogle projects
		else if(ei10 == 'iprojectsa')
		{
			var sl=parseInt(oID.iprojects.style.left);
			if(sl >= 0)
				fxf_fn_shrinkProjects(0.2,-1);
			else
				fxf_fn_growProjects(0.2,'','','');
		}
		else if((ei9 == 'pwaitscre') && (gSet.prjsl > 0))
			fxf_fn_shrinkProjects(0.2,-1);
		// Open/close projects folder
		else if((ei7 == 'pjsmfd_') || (ei7 == 'pjsmfi_'))
			fxf_fn_togglePJSMFolder(ei.substr(7),0);
		else if(ei7 == 'pjsmfn_')
			fxf_fn_togglePJSMFolder(ei.substr(7),1);
		// Project controls
		else if(ei6 == 'pjsmc_')
			fxf_fn_execPJSMControl(ei.substr(6));
		// Project search
		else if(ei == 'pjsmt_src')
			fxf_fn_searchPJSM(true);
		// Project action menu
		else if(ei6 == 'pjsmp_')
			fxf_fn_openPJSMMenu(event,ei.substr(6));
		// Project workflow action
		else if(ei6 == 'pjsma_')
		{
			var waa=ei.substr(6).split('_');
			var ntr=parseInt(waa[0]);
			var nac=parseInt(waa[1]);
			var mpn=waa[2];
//alert('Project workflow action: waa='+waa);
			if(ntr == 999)	// Mark project
			{
				if(!nac)
					mpn='-';
				fxf_fn_growProjects(0.2,mpn,'','');
			}
			else if((ntr > 0) && (ntr < 999) && (nac > 0))	// Call program function
			{
				fxf_fn_waitScreen('loading');
				fxf_fn_loadTR(ntr,'newtr&aktion='+nac+'&gpn='+mpn);
			}
		}

		// Click on men nav
		else if((ei == 'fxmenu1l') || (ei == 'fxmenu1r') || (ei == 'fxmenu2l') || (ei == 'fxmenu2r') || (ei == 'fxmenu3l') || (ei == 'fxmenu3r'))
			fxf_fn_menuNav(parseInt(ei.substr(6,1))-1, ei.substr(7,1));

		// Click on menu
		else if(ei4 == 'fxm_')
		{
			var id=ei.substr(4,ei.length-6);
			if(id.substr(ei.length-2,1) == '_')
				id=id.substr(0,ei.length-2);
			var ntr=0;
			var arg='';
			var ed=null;
			if(element.attributes && (typeof element.attributes['tr'] != 'undefined'))
			{
				ntr=parseInt(element.attributes['tr'].value);
				if(typeof element.attributes['arg'] != 'undefined')
					arg='&'+element.attributes['arg'].value;
			}
			else
			{
				ed=$('fxm_'+id+'_d');
				if(ed && ed.attributes && (typeof ed.attributes['tr'] != 'undefined'))
				{
					ntr=parseInt(ed.attributes['tr'].value);
					if(typeof ed.attributes['arg'] != 'undefined')
						arg='&'+ed.attributes['arg'].value;
				}
			}

			// Call transaction or get new menu
			if(ntr != 0)
			{
				fxf_fn_drawMenu('');
				if(ntr == 223)	// Filter
					fxf_fn_loadPTR(ntr,'newptr');
				else
				{
					fxf_fn_waitScreen('loading');
					fxf_fn_loadTR(ntr,'newtr'+arg)
				}
			}
			else
				fxf_fn_drawMenu(id);
		}

		// Click on logout
		else if(ei5 == 'fxlog')
			fxf_fn_loadTR(-1,'logout');

		// Click on action
		else if(ei6 == 'actreg')
		{
			if(element.className != 'actregi')
			{
				// Warn user absout action mode change if values have been edited and transaction is not in display mode
				var nact=parseInt(ei.substr(6,1));
				if((oFXP.changed > 0) && (oFXP.action > 1) && (oFXP.action != nact) && (oFXP.tr > 0))
				{
					fxf_fn_waitScreen('');
					fxf_fn_question(fxf_fn_getMessage(11,true), fxf_fn_getMessage(11,false), [fxf_fn_getText(10), fxf_fn_getText(11)], ['fxf_fn_resetChanged();fxf_fn_actionColumn('+parseInt(ei.substr(6,1))+',0)', ''], 100);
					return;
				}
				else
					fxf_fn_actionColumn(parseInt(ei.substr(6,1)),0);
			}
		}

		// Click on filemanager's automatic scale factor
		else if((ei == 'pdfs_fit') && !element.disabled)
			fxf_fn_pdfToggleFit.delay(oFXP.sdelay);

		// Click on design
		else if(ei9 == 'fxtaskdps')
		{
			var eia=ei.split('_');
			oID.fxtaskdp.style.display='none';
			if(eia[1].substr(0,3) == '000')
				fxf_fn_setDesign('');
			else
				fxf_fn_setDesign(eia[1]);
		}

		// Click on language
		else if(ei9 == 'fxtasklps')
		{
			// Warn user by transaction change if values have been edited and transaction is not in display mode
			if((oFXP.changed > 0) && (oFXP.action > 1) && (oFXP.tr > 0))
			{
				fxf_fn_waitScreen('');
				fxf_fn_question(fxf_fn_getMessage(6,true), fxf_fn_getMessage(6,false), [fxf_fn_getText(10), fxf_fn_getText(11)], ['fxf_fn_resetChanged();fxf_fn_waitScreen(\'reloading\');fxf_fn_setLanguage('+parseInt(ei.substr(9))+')', ''], 100);
				return;
			}
			else
			{
				fxf_fn_waitScreen('reloading');
				fxf_fn_setLanguage.delay(oFXP.sdelay, parseInt(ei.substr(9)));
			}
		}

		// Click on unit
		else if(ei9 == 'fxtaskups')
		{
			var eia=ei.split('_');
			var eiu=parseInt(eia[1]);
			fxf_fn_changeSetting(0,'punit',eiu);
			oSet.punit=fxf_fn_getUnit(eiu);
			if(oSet.punit != gSet.dunit)
			{
				var ounit=gSet.dunit;
				fxf_fn_checkUnit();
				fxf_fn_setUnit(ounit);
			}
			else
				oID.fxtaskup.style.display='none';
		}

		// Switch timespan format
		else if(ei7 == 'fxtaskf')
		{
			if(oID.fxtaskf.getOpacity() == 1.0)
			{
				if(oSet.timedec == 'j')
					fxf_fn_formatTimespan('n');
				else
					fxf_fn_formatTimespan('j');
			}
		}

		// Click on Yes/No or On/Off global settings
		else if((ei7 == 'set_yn_') || (ei7 == 'set_oo_'))
		{
			var lo=10;
			if(ei7 == 'set_oo_')
				lo=13;
			var sval=ei.substr(7);
			var sdct=$('div_'+ei.substr(4));
			var ssct='<span id="'+ei+'" class="switchadd"></span>';
			if(gSet[sval])
			{
				gSet[sval]=0;
				sdct.innerHTML='<span class="switchl0">'+fxf_fn_getText(lo)+'</span><span class="switchr1">'+fxf_fn_getText(lo+1)+'</span>'+ssct;
			}
			else
			{
				if(sval.substr(0,5) == 'amenu')
				{
					var amc=parseInt(sval.substr(5));
					var ama=$$('[id^="div_yn_amenu"]');
//alert('amenu\n\nama:\n'+ama+'\n\namc='+amc);
					for(var a=0; a<ama.length; a++)
					{
						if(ama[a].id == 'div_yn_amenu'+amc)
						{
							gSet[ama[a].id.substr(7)]=1;
							ama[a].innerHTML='<span class="switchl1">'+fxf_fn_getText(lo)+'</span><span class="switchr0">'+fxf_fn_getText(lo+1)+'</span><span id="set'+ama[a].id.substr(3)+'" class="switchadd" style="display:none;"></span>';
						}
						else
						{
							gSet[ama[a].id.substr(7)]=0;
							ama[a].innerHTML='<span class="switchl0">'+fxf_fn_getText(lo)+'</span><span class="switchr1">'+fxf_fn_getText(lo+1)+'</span><span id="set'+ama[a].id.substr(3)+'" class="switchadd"></span>';
						}
					}
				}
				else
				{
					gSet[sval]=1;
					sdct.innerHTML='<span class="switchl1">'+fxf_fn_getText(lo)+'</span><span class="switchr0">'+fxf_fn_getText(lo+1)+'</span>'+ssct;
				}
			}

			if((oFXP.tr == 189) && oID.sdg_container)
			{
				if(sval == 'showframes')
					fxf_fn_drawGantt();
				else if(sval == 'showhr')
					fxf_fn_drawProjectResources(-1);
				else if(sval == 'showlines')
					fxf_fn_drawLineNumbers();
			}
			if(sval == 'filter')
				fxf_fn_setFilter();
			if(sval.substr(0,5) == 'amenu')
			{
				var url=fxf_fn_gProgram('config_menu', fxf_fn_gParam());
				var pst='amenu='+sval.substr(5);
//alert('Change menu: sval='+sval+', gSet[sval]='+gSet[sval]+'\n\nurl='+url+'\n\npst:\n'+pst);

				new Ajax.Request(url,
				{
					method:'post', postBody:pst, asynchronous:asajax,
					onSuccess: function(transport)
					{
						var mtxt=transport.responseText;
//alert('mtxt:\n'+mtxt);
						if(mtxt.length > 0)
							fxf_fn_setMenu(mtxt);
					}
				});
			}

			// Save changed global setting
			fxf_fn_changeSetting(0,'set'+sval,gSet[sval]);
//alert('set'+sval+' -> '+gSet[sval]);
		}

		// Click on Yes/No or On/Off or Radiobutton special transaction settings
		else if((ei7 == 'str_yn_') || (ei7 == 'str_oo_') || (ei7 == 'str_ra_'))
		{
			if(ei7 == 'str_ra_')
			{
				var sval=ei.substr(7);
				var cval='';
				var io=sval.indexOf('_');
				if(io > 0)
				{
					cval=sval.substr(io+1);
					sval=sval.substr(0,io);
				}
				tSet[sval]=parseInt(element.value);
				document.getElementsByName(sval)[tSet[sval]].checked=true;

//alert('ei='+ei+', ei7='+ei7+', io='+io+', sval='+sval+', cval='+cval+', tSet['+sval+']='+tSet[sval]);
				if(cval.length)
				{
					ei='str_yn_'+cval;
					ei7=ei.substr(0,7);
					tSet[cval]=0;
				}

				// Save changed transaction setting
				fxf_fn_changeSetting(oFXP.tr,sval,tSet[sval]);
			}

			if((ei7 == 'str_yn_') || (ei7 == 'str_oo_'))
			{
				var lo=10;
				if(ei7 == 'str_oo_')
					lo=13;
				var sval=ei.substr(7);
				var sdct=$('div_'+ei.substr(4));
				var ssct='<span id="'+ei+'" class="switchadd"></span>';
				var ocb=$(sval);
//alert('ei='+ei+', ei7='+ei7+', lo='+lo+', sval='+sval+', sdct='+sdct+', ssct='+ssct+', ocb='+ocb);

				if(tSet[sval])
				{
					tSet[sval]=0;
					sdct.innerHTML='<span class="switchl0">'+fxf_fn_getText(lo)+'</span><span class="switchr1">'+fxf_fn_getText(lo+1)+'</span>'+ssct;
				}
				else
				{
					tSet[sval]=1;
					sdct.innerHTML='<span class="switchl1">'+fxf_fn_getText(lo)+'</span><span class="switchr0">'+fxf_fn_getText(lo+1)+'</span>'+ssct;
				}

				if(ocb)
				{
					if(tSet[sval])
						ocb.checked=true;
					else
						ocb.checked=false;
				}

				// Save changed transaction setting
				fxf_fn_changeSetting(oFXP.tr,sval,tSet[sval]);
			}

			if((oFXP.tr == 189) && oID.ndw_container)
				fxf_fn_drawHRPlanning();
		}

		// Question
		else if(ei5 == 'btn_q')
		{
			oID.iacont.style.display='none';
			oID.iact.style.display='none';
			var bc=parseInt(ei.substr(5));
			if((bc >= 0) && (bc < fstack.length) && fstack[bc].length)
				eval(fstack[bc]);
			fstack=[];
		}

		// PPSP Popup Line
		else if(ei7 == 'tdppsp_')
		{
			var trc=parseInt(ei.substr(7));
//alert('Click on line: '+trc);
			// ...Plan. Effort
			var rbe=$$('[name="ppspnef"]');
			if(rbe.length > trc)
				rbe[trc].checked=true;
			// ...Int. Plan. HRB
			if(((oSet.cost_see == 1) || (oSet.cost_see == 3)) && ((oSet.cost_btype == 'I') || (oSet.cost_btype == 'B')))
			{
				rbe=$$('[name="ppspnib"]');
				if(rbe.length > trc)
					rbe[trc].checked=true;
			}
			// ...Ext. Plan. HRB
			if(((oSet.cost_see == 2) || (oSet.cost_see == 3)) && ((oSet.cost_btype == 'E') || (oSet.cost_btype == 'B')))
			{
				rbe=$$('[name="ppspneb"]');
				if(rbe.length > trc)
					rbe[trc].checked=true;
			}
			fxf_fn_ppspClose(true);
		}

		else if((oID.searchpop.style.display == 'none') && (oID.selpop.style.display == 'none'))
		{
			if(oFXP.tr == 48)
			{
				if(ei7 == 'drslide')
					fxf_fn_dailyRateMove(element, event);
			}
			else if(oFXP.tr == 55)
			{
				if(ei == 'todo_art')
				{
					var tdod=$('todo_off');
					var tdpd=$('todo_pers');
					if(tdod && tdpd)
					{
						if(tdod.style.display == 'none')
						{
							tdod.style.display='';
							tdpd.style.display='none';
						}
						else
						{
							tdod.style.display='none';
							tdpd.style.display='';
						}
					}
				}
			}
			else if(oFXP.tr == 56)
			{
				if(ei == 'aenlcb')
					fxf_fn_cbToggleDetail(-1,true);
				else if(ei == 'ashrcb')
					fxf_fn_cbToggleDetail(-1,false);
				else if(ei6 == 'enlcb_')
				{
					var ln=parseInt(ei.substr(6));
					fxf_fn_cbToggleDetail(ln,true);
				}
				else if(ei6 == 'shrcb_')
				{
					var ln=parseInt(ei.substr(6));
					fxf_fn_cbToggleDetail(ln,false);
				}
			}
			else if((oFXP.tr == 83) && oID.trw_container)
			{
				// Click on "Edit extended time registration entry"
				if(ei.substr(0,9) == 'trw_edit_')
				{
					var dr=element.id.substr(9);
					var dt=dr.substr(0,8);
					var rw=dr.substr(9);
					fxf_fn_trwEdit(element,dt,rw);
				}
				// Click on "Delete this time registration entry"
				else if(ei.substr(0,11) == 'trw_delete_')
				{
					var dr=element.id.substr(11);
					var dt=dr.substr(0,8);
					var rw=dr.substr(9);
					fxf_fn_trwDelete(element,dt,rw);
				}
				// Click on "Delete all time registration entries on this day"
				else if(ei.substr(0,14) == 'trw_deleteall_')
				{
					var dr=element.id.substr(14);
					var dt=dr.substr(0,8);
					var rw=dr.substr(9);
					fxf_fn_trwDeleteAll(element,dt,rw);
				}
				// Click on "Undelete"
				else if(ei.substr(0,4) == 'ddl_')
				{
					var dr=element.id.substr(4,element.id.length-6);
					var dt=dr.substr(0,8);
					var rw=dr.substr(9);
					fxf_fn_trwUndelete(element,dt,rw);
				}
				// Click on "Enlarge Info"
				else if(ei == 'ienlarge')
				{
					oID.trinfo.style.display='';
					var wl=parseInt(oID.trinfo.style.width);
					var ws=parseInt(oID.trsinfo.style.width);
					oID.trsinfo.style.display='none';
					var l=parseInt(oID.trentries.style.left);
					oID.trentries.style.left=(l-ws+wl)+'px';
					oID.trsmain.style.left=(l-ws+wl)+'px';
					var w=parseInt(oID.trsum.style.width);
					oID.trsum.style.width=(w-ws+wl)+'px';
					fxf_fn_syncTWD();
				}
				// Click on "Shrink Info"
				else if(ei == 'ishrink')
				{
					oID.trsinfo.style.display='';
					var wl=parseInt(oID.trinfo.style.width);
					var ws=parseInt(oID.trsinfo.style.width);
					oID.trinfo.style.display='none';
					var l=parseInt(oID.trentries.style.left);
					oID.trentries.style.left=(l-wl+ws)+'px';
					oID.trsmain.style.left=(l-wl+ws)+'px';
					var w=parseInt(oID.trsum.style.width);
					oID.trsum.style.width=(w-wl+ws)+'px';
				}
			}
			else if(oFXP.tr == 104)
			{
				if(ei6 == 'invcp_')
				{
					var eip=ei.split('_');
					fxf_fn_gsInvoiceChange(parseInt(eip[1]),parseInt(eip[2]),parseInt(eip[3]));
				}
			}
			else if(((oFXP.tr == 107) || (oFXP.tr == 110)) && tSet.dms && tSet.dms.structure)
			{
//alert('ei='+ei);
				// Resize areas
				if((ei6 == 'sd_div') || (ei6 == 'di_div'))
					fxf_fn_dmsAreaResizeStart(element.id.substr(0,2), event);
				// Click on "Shrink Info"
				else if(ei == 'ishrink')
					fxf_fn_dmsInfoShrink();
				// Click on "Enlarge Info"
				else if(ei == 'ienlarge')
					fxf_fn_dmsInfoEnlarge();
				// Open/Close directory and read subdirectories
				else if(ei9 == 'dms_dira_')
				{
					var id=parseInt(ei.substr(9));
					if(tSet.dms.dirs[id].open == 'fo')
						tSet.dms.dirs[id].open='fc';
					else
						tSet.dms.dirs[id].open='fo';
					element.style.background='url(GFX/'+tSet.dms.dirs[id].open+'o.png)';
//alert('Click on directory arrow: id='+id+' -- new open='+tSet.dms.dirs[id].open);
					fxf_fn_showMessage('', '');
					fxf_fn_dmsRead('dirs', id, tSet.dms.dirs[id].open, '');
				}
				// Read directory content (click on directory structure folder)
				else if(ei9 == 'dms_dirn_')
				{
					var id=parseInt(ei.substr(9));
//alert('Click on directory name (dir structure): id='+id);
					fxf_fn_showMessage('', '');
					fxf_fn_dmsRead('dirs', id, '', '');
				}
				// Read directory content (click on document list folder) or select file (click on file)
				else if((ei6 == 'fname[') || (ei8 == 'd_fname[') || (ei10 == 'dms_col_fn'))
				{
					if(ei6 == 'fname[')
						var id=parseInt(ei.substr(6));
					else if(ei8 == 'd_fname[')
						var id=parseInt(ei.substr(8));
					else
						var id=parseInt(ei.substr(14));
					if(tSet.dms.files[id].type == 'd')
					{
//alert('Click on directory name (doc list): id='+id);
						if(tSet.dms.files[id].rights == 4)	// Open new subdirectory
							fxf_fn_dmsNewDir('open', -1);
						else
						{
							fxf_fn_showMessage('', '');
							fxf_fn_dmsRead('files', id, '', '');
						}
					}
					else if(!tSet.dms.save.length && (tSet.dms.rights[0] == tSet.dms.rights[1]))
					{
//alert('Click on file name: id='+id);
						var cb=$('nup_cb['+id+']');
						if(cb)
						{
							if(cb.checked && (!keyEvent || keyEvent.ctrlKey || !keyEvent.shiftKey))
								cb.checked=false;
							else
								cb.checked=true;
							fxf_fn_dmsSelectFile(id, false);
						}
					}
				}
				// Action (click on action button)
				else if((ei4 == 'dab_') || (ei5 == 'dabi_'))
				{
					if(ei4 == 'dab_')
						var act=ei.substr(4,3);
					else
						var act=ei.substr(5,3);
					if(act != 'dsw')	// "Display in a new window" has its own onclick to avoid popup blockers
					{
						var ec=$('dabi_'+act).className;
						if(ec == 'ticonb')	// Button active
						{
							if(act == 'fla')		// "Folder add"
								fxf_fn_dmsNewDir('add', -1);
							else if(act == 'ren')	// "Rename"
								fxf_fn_dmsRename();
							else
							{
								fxf_fn_showMessage('', '');
								fxf_fn_dmsAction(act, -1, '');
							}
						}
					}
				}
				// Save/Undo
				else if(ei8 == 'dms_btn_')
				{
					var bm=ei.substr(8);
					var cn=element.className;
					var ia=cn.indexOf('aactive');
					if(ia > 0)	// Button active
					{
//alert('Click on button ['+bm+']');
						fxf_fn_dmsSave(bm,'');
					}
				}
				// Search
				else if(ei == 'ssearchico')
					fxf_fn_dmsSearch.delay(oFXP.sdelay, null,false);
				// Filter
				else if(ei == 'sfiltertimg')
					fxf_fn_dmsFilter(0);
				// Toggle filter on/off
				else if(ei == 'sfilter01')
					fxf_fn_dmsFilter(1);
			}
			else if((oFXP.tr == 189) && oID.sdg_container)
			{
				fxf_fn_showStatusMessage(0);

				fxf_fn_editStop(event,false);
				oID.saction.style.display='none';
				oID.gaction.style.display='none';
				soi='';
				soii=-1;
				moi='';
				moii=-1;

				// Move project
				if((ei5 == 'gnuid') || (ei5 == 'gbuid') || (ei5 == 'nduid') || (ei5 == 'nnuid') || (ei5 == 'ntuid'))
				{
					// Gantt action bar
					if((ei5 == 'nduid') || (ei5 == 'ntuid'))
					{
						var pid=ei.substr(2);
						var p=fxf_fn_getKeyFromID('P',pid);
						if(moi != pid)
						{
							var p=fxf_fn_getKeyFromID('P',pid);
							if((p >= 0) && (p < pArray.length))
							{
								nar='cursor:help;" tooltip="<i class=red>'+fxf_fn_message('no-rights')+'</i>';

								var mrc=0;

								var h='';
 								if(p > 0)
								{
									if(pArray[p].rights & 4)
									{
										h += '<img id="gc'+pid+'" class="aactive" src="GFX/ic_cpy.png" style="margin-right:1px;">';
										mrc++;
									}
									else
										h += '<img class="ainactive" src="GFX/ic_cpy.png" style="margin-right:1px;'+nar+'">';

									if(pArray[p].rights & 8)
									{
										h += '<img id="gm'+pid+'" class="aactive" src="GFX/ic_mov.png" style="margin-right:1px;">';
										h += '<img id="gv'+pid+'" class="aactive" src="GFX/ic_movv.png" style="margin-right:1px;">';
										mrc++;
									}
									else
									{
										h += '<img class="ainactive" src="GFX/ic_mov.png" style="margin-right:1px;'+nar+'">';
										h += '<img class="ainactive" src="GFX/ic_movv.png" style="margin-right:1px;'+nar+'">';
									}
								}
								if((pArray[p].type == 'T') && (oSet.cost_see > 0) && (oSet.cost_btype != 'N'))
								{
									var bs='e';
									if(oSet.currency != '&euro;')
										bs='d';
									if(pArray[p].values.peffort && (pArray[p].rights & 8))
									{
										h += '<img id="gp'+pid+'" class="aactive" src="GFX/ic_'+bs+'calc.png" style="margin-right:1px;">';
										mrc++;
									}
									else
										h += '<img class="ainactive" src="GFX/ic_'+bs+'calc.png" style="margin-right:1px;'+nar+'">';
								}
								if(!hlarge && (hrheight > 0) && (pArray[p].type == 'T'))
								{
									if(pArray[p].rights & 8)
									{
										h += '<img id="hrenlargea" class="aactive" src="GFX/ic_res.png" style="margin-right:1px;" tooltip="';
										if(oFXP.lang == 1)
											h += 'Ressourcenplanungs-Bereich öffnen';
										else
											h += 'Open resource planning area';
										h += '">';
										mrc++;
									}
									else
										h += '<img class="ainactive" src="GFX/ic_res.png" style="margin-right:1px;'+nar+'">';
								}
								if(p > 0)
								{
									var ett=fxf_fn_checkDel(p);
									if(!ett.length)
									{
										h += '<img id="gd'+pid+'" class="aactive" src="GFX/ic_del.png">';
										mrc++;
									}
									else
										h += '<img class="ainactive" tooltip="<i class=red>'+ett+'</i>" style="cursor:help;" src="GFX/ic_del.png">';
								}

								h += '<img id="gu'+pid+'" class="aactive" src="GFX/ic_men.png"';
								if(mrc > 0)
									h += ' style="margin-left:8px;"';
								h += '>';
								mrc++;

								if(mrc > 0)
								{
									oID.gaction.innerHTML=h;
									oID.gaction.style.top=(fxf_fn_pTop(p)+parseInt(oID.sdg_container.style.top)+24-oID.gmain.scrollTop)+'px';
									oID.gaction.style.left=Math.max(0,mcx-14)+'px';
									oID.gaction.style.display='';

									oID.saction.style.display='none';
									soi='';
									soii=-1;

									moi=pid;
									moii=p;
								}
							}
							else
							{
								moi='';
								moii=-1;
							}
						}
					}
					fxf_fn_prjActionInit(element, 'move', event);
				}
				else if(ei5 == 'gsuid')
					fxf_fn_prjActionInit(element, 'enlarge-left', event);
				else if(ei5 == 'geuid')
					fxf_fn_prjActionInit(element, 'enlarge-right', event);

				// Copy or move project
				else if((ei5 == 'gcuid') || (ei5 == 'gmuid') || (ei5 == 'gvuid'))
					fxf_fn_actInit(element, event);

				// Force PPSP Popup
				else if((ei5 == 'gpuid') || (ei5 == 'bcalc'))
				{
					if(ei5 == 'gpuid')
						var pc=fxf_fn_getKeyFromID('P',ei.substr(2));
					else
						var pc=parseInt(ei.substr(5));
					var pop=fxf_fn_PPSPPopup('',pc,1);
				}

				// Create dependency
				else if(ei4 == 'gdpl')
					fxf_fn_depInit(element, event);

				// Move milestone
				else if(ei5 == 'giuid')
					fxf_fn_mstInit(element, event);

				// Zoom
				else if(ei7 == 'zslider')
					fxf_fn_zoomMove(element, event);

				// Position overview
				else if(ei == 'coverview')
					fxf_fn_overInit(element, event);

				// Enlarge gantt area
				if(ei6 == 'gleft2')
					fxf_fn_enlargeGantt(-2);
				else if((ei6 == 'gleft1') || (ei6 == 'gjumps'))
					fxf_fn_enlargeGantt(-1);
				else if((ei7 == 'gright1') || (ei6 == 'gjumpe'))
					fxf_fn_enlargeGantt(1);
				else if(ei7 == 'gright2')
					fxf_fn_enlargeGantt(2);

				// Undo/Redo
				else if(ei4 == 'undo')
					fxf_fn_undo(true);
				else if(ei4 == 'redo')
					fxf_fn_redo(true);

				// Jump to positions
				else if(ei4 == 'jump')
					fxf_fn_jumpPosition(element.id.substr(4,2));

				// Optimize zoom
				else if(ei6 == 'zoomop')
					fxf_fn_zoomOptimize(true);

				// Toggle sub projects
				else if(ei8 == 'sptoggle')
					fxf_fn_toggleSubProjects(false);
				else if(ei9 == 'spftoggle')
					fxf_fn_toggleSubProjects(true);

				// Toggle overview
				else if(ei6 == 'toverv')
					fxf_fn_toggleOverview();

				// Shrink areas...
				else if(ei7 == 'sshrink')	// ...Project structure area
				{
					slarge=false;
					fxf_fn_resize();
				}
				else if(ei7 == 'dshrink')	// ...Project detail area
				{
					dlarge=false;
					fxf_fn_resize();
				}
				else if(ei7 == 'gshrink')	// ...Project gantt area
				{
					glarge=false;
					fxf_fn_resize();
				}
				else if(ei8 == 'hrshrink')	// ...HR area
					fxf_fn_HRShrink(false);

				// Enlarge areas...
				else if(ei8 == 'senlarge')	// ...Project structure area
				{
					slarge=true;
					fxf_fn_showArea(1,true);
				}
				else if(ei8 == 'denlarge')	// ...Project detail area
				{
					dlarge=true;
					fxf_fn_showArea(2,true);
				}
				else if(ei8 == 'genlarge')	// ...Project gantt area
				{
					glarge=true;
					fxf_fn_showArea(3,true);
				}
				else if(ei9 == 'hrenlarge')	// ...HR area
					fxf_fn_HREnlarge(false);

				// HR workload settings
				else if(ei5 == 'wlscb')
				{
					if(element.checked)
						tSet[ei6]=0;
					else
						tSet[ei6]=1;

					fxf_fn_changeSetting(oFXP.tr,ei6,tSet[ei6]);
					fxf_fn_drawHRPlanning();
				}
				else if(ei6 == 'wlsras')
				{
					tSet[ei6]=parseInt(element.value);

					$('wlscbs').checked=true;
					tSet.wlscbs=1;

					fxf_fn_changeSetting(oFXP.tr,'wlscbs|'+ei6,'1|'+tSet[ei6]);
					fxf_fn_drawHRPlanning();
				}

				// Toggle HR Settings Popup
				else if(ei5 == 'hrset')
					fxf_fn_setPopup(true);

				// Change HR Column Sort
				else if(ei6 == 'hrpth_')
					fxf_fn_HRPoolSortColumn(ei.substr(6));

				// Open/Close projects
				else if(ei5 == 'fouid')	// ...Close
					fxf_fn_folderProject(-1, element.id.substr(2), true);
				else if(ei5 == 'fcuid')	// ...Open
					fxf_fn_folderProject(1, element.id.substr(2), true);

				// Hide/Show projects
				else if(ei5 == 'fhuid')
				{
					var pc=fxf_fn_getKeyFromID('P', element.id.substr(2));
					var pm=-2;
					if((pArray[pc].celement != null) && (pArray[pc].celement.style.display == 'none'))
						pm=2;
					fxf_fn_folderProject(pm, element.id.substr(2), true);
				}

				// Decrease/Increase project level
				else if(ei7 == 'lvlduid')	// ...Decrease
					fxf_fn_newLevel(-1, element.id.substr(4));
				else if(ei7 == 'lvliuid')	// ...Increase
					fxf_fn_newLevel(1, element.id.substr(4));

				// Copy projects/tasks
				else if(ei4 == 'copy')
				{
					mcl=pArray[pAct].left;
					fxf_fn_copyProjects(parseInt(element.id.substr(4)), -1);
				}

				// Mark milestone
				else if((ei5 == 'giuid') || (ei5 == 'nsuid') || (ei5 == 'nmuid'))
				{
					pAct=fxf_fn_getKeyFromID('P', element.id.substr(2));
					dAct=-1;
					fxf_fn_markObject(false);
					mka='G';
					fxf_fn_showStatusMessage(0);
				}

				// Mark dependency
				else if(ei2 == 'dp')
				{
					pAct=-1;
					dAct=fxf_fn_getKeyFromID('D', element.id.substr(3));
					fxf_fn_markObject(false);
					mka='G';
					fxf_fn_showStatusMessage(0);
				}

				// Remove project/milestone or dependency marker
				else if((ei5 == 'gbars') || (ei6 == 'pframe') || (ei6 == 'glmark') || (ei6 == 'gcmark'))
				{
					pAct=-1;
					dAct=-1;
					fxf_fn_markObject(false);
					mka='';
					fxf_fn_showStatusMessage(0);
				}

				// Add...
				else if(ei5 == 'ppadd')	// ...project
					fxf_fn_addProject(parseInt(element.id.substr(5)), 'P');
				else if(ei5 == 'ptadd')	// ...task
					fxf_fn_addProject(parseInt(element.id.substr(5)), 'T');
				else if(ei5 == 'pmadd')	// ...milestone
					fxf_fn_addProject(parseInt(element.id.substr(5)), 'M');
				else if(ei5 == 'paadd')	// ...dependency A
					fxf_fn_addPreviousDependency(parseInt(element.id.substr(5)));
				else if(ei5 == 'pbadd')	// ...dependency B
					fxf_fn_addNextDependency(parseInt(element.id.substr(5)));

				// Delete...
				else if((ei4 == 'pdel') || (ei5 == 'gduid'))	// ...project/task/milestone
				{
					var pc=0;
					if(ei4 == 'pdel')
						pc=parseInt(ei.substr(4));
					else
						pc=fxf_fn_getKeyFromID('P', ei.substr(2));
					if(pc != 0)
					{
						var ett=fxf_fn_checkDel(pc);
						if(!ett.length)
							fxf_fn_delProject(pc, true);
					}
				}
				else if(ei4 == 'mdel')	// ...milestones
				{
					var pc=parseInt(element.id.substr(4));
					fxf_fn_delMilestones(pc);
				}
				else if(ei4 == 'ddel')	// ...dependency
					fxf_fn_delDependency(parseInt(element.id.substr(4)), true);
				else if(ei5 == 'padel')	// ...all dependencies
				{
					var pc=parseInt(element.id.substr(5));
					fxf_fn_delDependencies(pc, '');
				}
				else if(ei5 == 'psdel')	// ...previous dependencies
				{
					var pc=parseInt(element.id.substr(5));
					fxf_fn_delDependencies(pc, 'S');
				}
				else if(ei5 == 'pfdel')	// ...next dependencies
				{
					var pc=parseInt(element.id.substr(5));
					fxf_fn_delDependencies(pc, 'F');
				}

				// Rename...
				else if(ei4 == 'pren')	// ...project
					fxf_fn_editText($('nt'+pArray[parseInt(element.id.substr(4))].uid));
				else if(ei4 == 'mren')	// ...milestone
					fxf_fn_editText($('nm'+pArray[parseInt(element.id.substr(4))].uid));

				// Change...
				else if(ei3 == 'dct')	// ...dependency
					fxf_fn_changeDependency(parseInt(element.id.substr(5)), element.id.substr(3,2).toUpperCase());

				// Switch...
				else if(ei3 == 'dst')	// ...dependency
					fxf_fn_switchDependency(parseInt(element.id.substr(5)));
				else if(ei4 == 'styp')	// ...project type
					fxf_fn_switchProjectType(fxf_fn_getKeyFromID('P', element.id.substr(4)),false);
				else if(ei4 == 'mstf')	// ...milestone type: fix
					fxf_fn_switchMilestoneType(parseInt(element.id.substr(4)), 0);
				else if(ei4 == 'msts')	// ...milestone type: start
					fxf_fn_switchMilestoneType(parseInt(element.id.substr(4)), -1);
				else if(ei4 == 'mste')	// ...milestone type: end
					fxf_fn_switchMilestoneType(parseInt(element.id.substr(4)), 1);


				// Menu...
				else if(ei5 == 'guuid')
				{
					var pc=fxf_fn_getKeyFromID('P', ei.substr(2));
					fxf_eh_contextMenu(event,element);
				}

				// Resize areas
				else if((ei6 == 'sd_div') || (ei6 == 'dg_div') || (ei6 == 'mh_div') || (ei6 == 'nd_div') || (ei6 == 'dw_div'))
					fxf_fn_areaResize(element.id.substr(0,2), event);

				// Structure action bar
				else if((ei5 == 'snuid') || (ei5 == 'stuid'))
				{
					// Mark project/milestone
					if(ei5 == 'stuid')
					{
						pAct=fxf_fn_getKeyFromID('P', element.id.substr(2));
						dAct=-1;
						fxf_fn_markObject(false);
						mka='S';
					}
					var pid=ei.substr(2);
					if(soi != pid)
					{
						var p=fxf_fn_getKeyFromID('P',pid);
						if((p >= 0) && (p < pArray.length) && (pArray[p].rights >= 4))
						{
							soi=pid;
							soii=p;
							if(!pArray.length)
								oID.saction.innerHTML=fxf_fn_drawAdd(-1);
							else if(pArray[p].type == 'M')
								oID.saction.innerHTML=fxf_fn_drawDel(p);
							else
								oID.saction.innerHTML=fxf_fn_drawAdd(p)+fxf_fn_drawSwitchProjectType(p)+fxf_fn_drawDecLevel(p)+fxf_fn_drawIncLevel(p)+fxf_fn_drawCopy(p)+fxf_fn_drawBCalc(p)+fxf_fn_drawDel(p);

							oID.saction.style.top=(fxf_fn_pTop(p)+parseInt(oID.sdg_container.style.top)+24-oID.gmain.scrollTop)+'px';
							oID.saction.style.left=Math.max(0,mcx-14)+'px';
							oID.saction.style.display='';

							oID.gaction.style.display='none';
							moi='';
							moii=-1;
						}
						else
						{
							soi='';
							soii=-1;
						}
					}
				}
			}
			else
			{
				if(oFXP.tr == 217)	// Form designer
				{
					if(ei3 == 'ic_')	// Click on top toolbar icon - show/hide window
					{
						var div=$('conf_'+ei.substr(3)+'_div');
						if(div.style.display == 'none')
						{
							div.style.display='';
							$('conf_status_div').innerHTML='&nbsp;';
						}
						else
							div.style.display='none';
					}
					else if(ei4 == 'icg_')	// Click on below toolbar icon - show command
					{
						var std=$('conf_status_div');
						var div=ei.substr(4);
						if(div == 'pic')
							std.innerHTML='<b class="blue">^P</b> <b>pic</b> <font class="lightgrey">|</font> x <font class="lightgrey">|</font> y <font class="lightgrey">|</font> sf <font class="lightgrey">|</font> align <font class="lightgrey">|</font> width <font class="lightgrey">|</font> height';
						else if(div == 'txt')
							std.innerHTML='<b class="blue">^T</b> <b>text</b> <font class="lightgrey">|</font> x <font class="lightgrey">|</font> y <font class="lightgrey">|</font> sf <font class="lightgrey">|</font> align <font class="lightgrey">|</font> mode <font class="lightgrey">|</font> lw';
						else if(div == 'box')
							std.innerHTML='<b class="blue">^B</b> x1 <font class="lightgrey">|</font> y1 <font class="lightgrey">|</font> x2 <font class="lightgrey">|</font> y2 <font class="lightgrey">|</font> rgb <font class="lightgrey">|</font> fb <font class="lightgrey">|</font> wd <font class="lightgrey">|</font> bd <font class="lightgrey">|</font> lw';
						else if(div == 'rec')
							std.innerHTML='<b class="blue">^R</b> x1 <font class="lightgrey">|</font> y1 <font class="lightgrey">|</font> x2 <font class="lightgrey">|</font> y2 <font class="lightgrey">|</font> wd <font class="lightgrey">|</font> bd <font class="lightgrey">|</font> lw';
						else if(div == 'lin')
							std.innerHTML='<b class="blue">^L</b> x1 <font class="lightgrey">|</font> y1 <font class="lightgrey">|</font> x2 <font class="lightgrey">|</font> y2 <font class="lightgrey">|</font> wd <font class="lightgrey">|</font> bd <font class="lightgrey">|</font> lw <font class="lightgrey">|</font> rgb';
					}
				}
				else if(oFXP.tr == 218)	// Start value overtime
				{
					if(ei5 == 'icopy')
						fxf_fn_copyCarry(element);
				}
			}
		}
	}
}

function fxf_eh_mouseOver(event)
{
	var element=$(Event.element(event));
	var check_tooltip=false;
	var ei='';
	if(element && element.id)
		ei=element.id;
	var ff='';
	if(element && element.className && ((element.className.substr(0,3) == 'fxf') || (element.className.substr(0,3) == 'fmf')))
		ff=element.className.substr(3,2);
//fxf_fn_taskMsg('fxf_eh_mouseOver: element.id='+element.id);
fxf_fn_writeDebug('info', 'fxf_eh_mouseOver: '+ei+' ('+ff+')');

	if(ff.length && ei.length)
	{
		var ei12=ei.substr(0,12);
		if((ei12 != 'txtpoptoggle') && (oID.txtpop.style.display == 'none'))
			oID.txtpoptoggle.style.display='none';
		if((ei12 != 'selpoptoggle') && (oID.selpop.style.display == 'none'))
			oID.selpoptoggle.style.display='none';
	}

	// Mark table row?
	if(gSet.trmark && oID.trmarkerl && oID.trmarkert && oID.trmarkerr && oID.trmarkerb)
	{
		var etrdisp=false;
		var etrdispt=false;
		var etrdispb=false;
		var etrdispl=false;
		var etrdispr=false;
		var etra=[];
		etra[0]=element;
		while(true)
		{
			var uetr=etra[etra.length-1].up('tr');
			if(uetr && (uetr != 'undefined'))
			{
				etra[etra.length]=uetr;
				if(uetr.id == 'mttr1')
					break;
			}
			else
				break;
		}
		var etr=etra.length-1;
		if((etr >= 0) && (etra[etr].id == 'mttr1'))
		{
			etr--;
			if((etr >= 0) && (etra[etr].nodeName.toLowerCase() == 'tr'))
			{
				var etrid=etra[etr].id;
				if(!etrid.length || (etrid.substr(0,10) != 'listheader'))
				{
					var etrbcr=fxf_fn_getBCR(etra[etr]);
					if(etrbcr.top)
					{
						etrdisp=true;

						var flh=$('flistheader');
						if(flh)
							var tmin=fxf_fn_getBCR(flh).bottom;
						else
							var tmin=dim.wd.top;

						var tmax=dim.wd.bottom+2;

						etrbcr.left -= 2;
						etrbcr.top -= 1;
						etrbcr.right += 2;
						etrbcr.bottom += 1;

						if(etrbcr.top >= tmin)
							etrdispt=true;
						else
							etrbcr.top=tmin;

						if(etrbcr.bottom <= tmax)
							etrdispb=true;
						else
							etrbcr.bottom=tmax;

						if(etrbcr.left >= 15)
							etrdispl=true;
						else
							etrbcr.left=15;

						if(etrbcr.right <= dim.sd.swidth-37)
							etrdispr=true;
						else
							etrbcr.right=dim.sd.swidth-37;

						etrbcr.width=etrbcr.right-etrbcr.left;
						etrbcr.height=etrbcr.bottom-etrbcr.top;

						// ...left
						oID.trmarkerl.style.left=etrbcr.left+'px';
						oID.trmarkerl.style.top=etrbcr.top+'px';
						oID.trmarkerl.style.height=etrbcr.height+'px';

						// ...top
						oID.trmarkert.style.left=etrbcr.left+'px';
						oID.trmarkert.style.top=etrbcr.top+'px';
						oID.trmarkert.style.width=etrbcr.width+'px';

						// ...right
						oID.trmarkerr.style.left=etrbcr.right+'px';
						oID.trmarkerr.style.top=etrbcr.top+'px';
						oID.trmarkerr.style.height=etrbcr.height+'px';

						// ...bottom
						oID.trmarkerb.style.left=etrbcr.left+'px';
						oID.trmarkerb.style.top=etrbcr.bottom+'px';
						oID.trmarkerb.style.width=etrbcr.width+'px';
//fxf_fn_taskMsg('l='+etrbcr.left+', t='+etrbcr.top+', r='+etrbcr.right+', b='+etrbcr.bottom+' -- swidth='+dim.sd.swidth+', awidth='+dim.sd.awidth+', pwidth='+dim.sd.pwidth);
					}
				}
			}
		}
		if(etrdisp)
		{
			if(etrdispt)
				oID.trmarkert.style.display='';
			else
				oID.trmarkert.style.display='none';
			if(etrdispb)
				oID.trmarkerb.style.display='';
			else
				oID.trmarkerb.style.display='none';
			if(etrdispl)
				oID.trmarkerl.style.display='';
			else
				oID.trmarkerl.style.display='none';
			if(etrdispr)
				oID.trmarkerr.style.display='';
			else
				oID.trmarkerr.style.display='none';
		}
		else
		{
			oID.trmarkerl.style.display='none';
			oID.trmarkert.style.display='none';
			oID.trmarkerr.style.display='none';
			oID.trmarkerb.style.display='none';
		}
	}

	var maxlength=0;
	if(element.readAttribute('maxlength'))
		maxlength=parseInt(element.readAttribute('maxlength'));

	if(oFXP.tr && ff.length && (oID.txtpop.style.display == 'none') && (oID.iact.style.display == 'none') && !element.disabled && (((ff == 'tx') && element.attributes && element.attributes['enlarge'] && !element.attributes['ud']) || (ff == 'ta'))) // Textfield/Textarea
	{
		fxf_fn_textPopup(0,element);
		check_tooltip=true;
	}
	else if((ff == 'sm') && (oID.selpop.style.display == 'none'))	// Multiselect Container
	{
		var msl=element.childElements();
		var mse=fxf_fn_getField(element,false);
		if(msl && msl.length)
			fxf_fn_multiselToggle(1,mse);
		check_tooltip=true;
	}
	else if(ei.length && (ei.substr(0,5) == 'fspmo') && (oID.selpop.style.display == 'none'))	// Multiselect Entry
	{
		var msl=element.up().childElements();
		var mse=fxf_fn_getField(element.up().up(),false);
		if(msl && msl.length)
			fxf_fn_multiselToggle(1,mse);
		check_tooltip=true;
	}
	else if(ei.length && (oFXP.tr == 189) && oID.sdg_container && (oID.iainfo.style.display == 'none'))
	{
		if(!mcheck && !mmode)
		{
			var ei4=ei.substr(0,4);
			var ei5=ei.substr(0,5);
			var ei6=ei.substr(0,6);

			if((ei5 == 'gsuid') || (ei5 == 'geuid'))
			{
				var pc=fxf_fn_getKeyFromID('P',ei.substr(2));
				fxf_fn_tooltip(5);
			}
			else if(ei5 == 'gdpla')
				fxf_fn_tooltip(6);
			else if(ei5 == 'gdplb')
				fxf_fn_tooltip(7);
			else if(ei5 == 'gcuid')
			{
				var pc=fxf_fn_getKeyFromID('P',ei.substr(2));
				fxf_fn_tooltip(8);
			}
			else if(ei5 == 'gmuid')
			{
				var pc=fxf_fn_getKeyFromID('P',ei.substr(2));
				fxf_fn_tooltip(9);
			}
			else if(ei5 == 'gvuid')
			{
				var pc=fxf_fn_getKeyFromID('P',ei.substr(2));
				fxf_fn_tooltip(13);
			}
			else if(ei5 == 'gpuid')
			{
				var pc=fxf_fn_getKeyFromID('P',ei.substr(2));
				fxf_fn_tooltip(14);
			}
			else if(ei5 == 'gduid')
			{
				var pc=fxf_fn_getKeyFromID('P',ei.substr(2));
				fxf_fn_tooltip(10);
			}
			else if(ei5 == 'guuid')
			{
				var pc=fxf_fn_getKeyFromID('P',ei.substr(2));
				fxf_fn_tooltip(15);
			}
			else if(ei5 == 'ntuid')
			{
				var pid=ei.substr(2);
				var p=fxf_fn_getKeyFromID('P',pid);
				fxf_fn_tooltip(11);
			}
			else if(ei5 == 'giuid')
			{
				var pc=fxf_fn_getKeyFromID('P',ei.substr(2));
				fxf_fn_tooltip(24);
			}
			else if((ei5 == 'nsuid') || (ei5 == 'nmuid'))
			{
				var pc=fxf_fn_getKeyFromID('P',ei.substr(2));
				fxf_fn_tooltip(25);
			}
			else if(ei5 == 'tover')
				fxf_fn_tooltip(26);
			else if(ei5 == 'cover')
				fxf_fn_tooltip(27);
			else if(ei6 == 'hrpth_')
			{
				fxf_fn_HRPoolMouseOver(ei.substr(6));
				check_tooltip=true;
			}
			else if(ei5 == 'wldc_')
			{
				var rw=ei.substr(5).split('_');
				var tt=fxf_fn_HRWorkloadTooltip(parseInt(rw[0]), parseInt(rw[1]));
				fxf_fn_tooltip(-1,tt);
			}
			else if(ei5 == 'wlds_')
			{
				var tt=fxf_fn_HRWorkloadSumTooltip(parseInt(ei.substr(5)));
				fxf_fn_tooltip(-1,tt);
			}
			else if((cmenu < 1) && ((ei5 == 'ppadd') || (ei5 == 'ptadd') || (ei4 == 'styp') || (ei4 == 'lvld') || (ei4 == 'lvli') || (ei4 == 'copy') || (ei4 == 'move') || (ei4 == 'bcal') || (ei4 == 'pdel')))
			{
				if(ei5 == 'ppadd')
				{
					var pc=parseInt(ei.substr(5));
					if(!pArray.length)
						fxf_fn_tooltip(-1, fxf_fn_getMenuMessage(1,pc));	// Create new main project
					else
						fxf_fn_tooltip(-1, fxf_fn_getMenuMessage(3,pc));	// Add new subproject
				}
				else if(ei5 == 'ptadd')
				{
					var pc=parseInt(ei.substr(5));
					fxf_fn_tooltip(-1, fxf_fn_getMenuMessage(4,pc));	// Add new task
				}
				else if(ei4 == 'styp')
				{
					var pc=fxf_fn_getKeyFromID('P', ei.substr(4));
					fxf_fn_tooltip(-1, fxf_fn_getMenuMessage(21,pc));	// Switch type
				}
				else if(ei4 == 'lvld')
				{
					var pc=fxf_fn_getKeyFromID('P', ei.substr(4));
					fxf_fn_tooltip(-1, fxf_fn_getMenuMessage(6,pc));	// Decrease level
				}
				else if(ei4 == 'lvli')
				{
					var pc=fxf_fn_getKeyFromID('P', ei.substr(4));
					fxf_fn_tooltip(-1, fxf_fn_getMenuMessage(5,pc));	// Increase level
				}
				else if(ei4 == 'copy')
					fxf_fn_tooltip(-1, fxf_fn_getMenuMessage(19,pAct));	// Copy project
				else if(ei4 == 'bcal')
					fxf_fn_tooltip(14);									// Recalc task
				else if(ei4 == 'pdel')
				{
					var pc=parseInt(ei.substr(4));
					fxf_fn_tooltip(-1, fxf_fn_getMenuMessage(14,pc));	// Delete task/project
				}
			}
			else if(!pArray.length && (oID.cmenu.style.display == 'none') && ((ei == 'gbars') || (ei == 'pframe')))
				fxf_fn_tooltip(21);
			else
				check_tooltip=true;
		}
		else
			check_tooltip=true;
	}
	else
		check_tooltip=true;

	if(check_tooltip)
	{
		if(element && (!element.style || !element.style.display || (element.style.display != 'none')) && element.attributes && (typeof element.attributes['tooltip'] != 'undefined'))
			fxf_fn_tooltip(-1, element.attributes['tooltip'].value);
		else if(element && element.up)
		{
			var eutt=element.up('[tooltip]');
			if(eutt && (!eutt.style || !eutt.style.display || (eutt.style.display != 'none')))
				fxf_fn_tooltip(-1, eutt.attributes['tooltip'].value);
			else
				fxf_fn_tooltip(0);
		}
	}

	if(ei.length && oID.fxtaskpp && oID.fxtaskpp.style)
	{
		if(ei.substr(0,7) == 'fxtaskp')
			oID.fxtaskpp.style.display='';
		else if(oID.fxtaskpp.style.display != 'none')
			oID.fxtaskpp.style.display='none';

		if(ei.substr(0,7) == 'fxtaskd')
		{
			var dsgea=$$('[id^="fxtaskdps_"]');
			if(dsgea && dsgea.length)
			{
				for(var dc=0; dc<dsgea.length; dc++)
				{
					if(dsgea[dc].id.substr(dsgea[dc].id.length-5,5) == '_span')
						dsgea[dc].className='black';
				}
				if(oFXP.design.length)
					var tddsg=$('fxtaskdps_'+oFXP.design+'_span');
				else
					var tddsg=$('fxtaskdps_000default_span');
				if(tddsg)
					tddsg.className='blue';
			}
			var fxds=fxf_fn_getBCR(oID.fxtaskd);
			oID.fxtaskdp.style.right=(Math.max(0,dim.sd.pwidth-10-fxds.right))+'px';
			oID.fxtaskdp.style.display='';
		}
		else if(oID.fxtaskdp.style.display != 'none')
		{
			oID.fxtaskdp.style.display='none';
		}

		// Language
		if((ei.substr(0,7) == 'fxtaskl') && (oFXP.tr > 0))
		{
			var fxds=fxf_fn_getBCR(oID.fxtaskl);
			oID.fxtasklp.style.right=(Math.max(0,dim.sd.pwidth-10-fxds.right))+'px';

			oID.fxtasklp.style.display='';
		}
		else if(oID.fxtasklp.style.display != 'none')
			oID.fxtasklp.style.display='none';

		if(gSet.cunit && (ei.substr(0,7) == 'fxtasku'))
		{
			var dul=$('fxtaskupl');
			var ul=0;
			if(dul)
				ul=parseInt(dul.value);
			if(!ul || (ul != oFXP.lang))
			{
				var tul='<input id="fxtaskupl" type="hidden" value="'+oFXP.lang+'">';
				tul += '<table border="0" cellpadding="0" cellspacing="2">';
				for(var u=0; u<oSet.unitselect.length; u++)
				{
					var uid=parseInt(oSet.unitselect[u]);
					if(uid && (uid != 856))
					{
						var suid='fxtaskups_'+uid;
						tul += '<tr id="'+suid+'_tr" style="cursor:pointer;"><td>&nbsp;<span id="'+suid+'_span" style="cursor:pointer;" class="black">'+oSet.unitlit[u]+'</span></td></tr>';
					}
				}
				tul += '</table>';
				oID.fxtaskup.innerHTML=tul;
			}
			var uea=$$('[id^="fxtaskups_"]');
			if(uea && uea.length)
			{
				for(var uc=0; uc<uea.length; uc++)
				{
					if(uea[uc].id.substr(uea[uc].id.length-5,5) == '_span')
						uea[uc].className='black';
				}
				var tdu=$('fxtaskups_'+oSet.unitselect[gSet.dunit]+'_span');
				if(tdu)
					tdu.className='blue';
			}
			var fxds=fxf_fn_getBCR(oID.fxtasku);
			oID.fxtaskup.style.right=(Math.max(0,dim.sd.pwidth-10-fxds.right))+'px';
			oID.fxtaskup.style.display='';
		}
		else if(oID.fxtaskup.style.display != 'none')
		{
			oID.fxtaskup.style.display='none';
		}
	}
}

function fxf_eh_mouseOut(event)
{
	var element=$(Event.element(event));
	if(!element || !element.id)
		return;
//fxf_fn_taskMsg('fxf_eh_mouseOut: element.id='+element.id);

	var ei=element.id;
//fxf_fn_writeDebug('info', 'fxf_eh_mouseOut: '+element.id);

	var ei6=ei.substr(0,6);

	if(ei6 == 'hrpth_')
		fxf_fn_HRPoolMouseOut(ei.substr(6));

	fxf_fn_tooltip(0);
}

function fxf_fn_HRPoolMouseOver(hi)
{
	var sn=tSet.hrcsort.substr(0,2);
	var sd=parseInt(tSet.hrcsort.substr(2));
	var et=hi.substr(0,2);
	var cn=hi.substr(2,2);
	var td=$('hrpth_td'+cn);
	var dc=$('hrpth_dc'+cn);
	var tw=parseInt(td.width);
//alert('fxf_fn_HRPoolMouseOver('+hi+')\nsn='+sn+', sd='+sd+'\net='+et+', cn='+cn+', td='+td+', dc='+dc+' - tw='+tw);

	var pl=-9;
	var wa=3;
	if(cn == 'nm')
	{
		pl=-27;
		wa=21;
	}

	// Check for filter icon in column
	var iw=0;
	var fid=$('hr'+cn+'f');
	if(fid)
	{
		iw=14;
		fid.src='GFX/cm_smfh.png';
	}

	var si='u';
	if(((sn == cn) && !sd) || ((sn != cn) && ((cn == 'ah') || (cn == 'sp'))))
		si='d';
	var h='<div style="position:absolute;left:'+pl+'px;top:-9px;width:'+(tw+wa-iw)+'px;height:19px;background:url(GFX/cm_smk.png);"><img src="GFX/cm_sm'+si+'.png" style="position:absolute;right:-15px;top:0;width:15px;height:19px;z-index:256;">';
	if(sn == cn)
	{
		si='d';
		if(sd)
			si='u';
		h += '<img src="GFX/cm_smm'+si+'.png" style="position:absolute;left:'+(Math.floor((tw+wa-iw)/2)+4)+'px;top:1px;">';
	}
	h += '</div>';

	dc.innerHTML=h;
}

function fxf_fn_HRPoolMouseOut(hi)
{
	var sn=tSet.hrcsort.substr(0,2);
	var sd=parseInt(tSet.hrcsort.substr(2));
	var et=hi.substr(0,2);
	var cn=hi.substr(2,2);
	var td=$('hrpth_td'+cn);
	var dc=$('hrpth_dc'+cn);
	var tw=parseInt(td.width);

	var pl=-9;
	var wa=3;
	if(cn == 'nm')
	{
		pl=-27;
		wa=21;
	}

	// Check for filter icon in column
	var iw=0;
	var fid=$('hr'+cn+'f');
	if(fid)
	{
		iw=14;
		if(tSet.hrcsort.substr(0,2) == cn)
			var im='cm_smf';
		else
			var im='cm_scf';
		fid.src='GFX/'+im+'.png';
	}

	var h='';
	if(sn == cn)
	{
		h += '<div style="position:absolute;left:'+pl+'px;top:-9px;width:'+(tw+wa-iw)+'px;height:19px;background:url(GFX/cm_sma.png);"><img src="GFX/cm_sme.png" style="position:absolute;right:-15px;top:0;width:15px;height:19px;">';
		var si='d';
		if(sd)
			si='u';
		h += '<img src="GFX/cm_smm'+si+'.png" style="position:absolute;left:'+(Math.floor((tw+wa-iw)/2)+4)+'px;top:1px;">';
		h += '</div>';
	}

	dc.innerHTML=h;
}

function fxf_eh_dragStart(event)
{
	var element=$(Event.element(event));
	if(!element || !element.id)
		return;
//fxf_fn_taskMsg('fxf_eh_dragStart: element.id='+element.id);
	event.stopPropagation();
	event.preventDefault();
}

function fxf_eh_dragEnter(event)
{
	var element=$(Event.element(event));
	if(!element || !element.id)
		return;
//fxf_fn_taskMsg('fxf_eh_dragEnter: element.id='+element.id);
	event.stopPropagation();
	event.preventDefault();
}

function fxf_eh_dragOver(event)
{
	var element=$(Event.element(event));
	if(!element || !element.id)
		return;
//fxf_fn_taskMsg('fxf_eh_dragOver: element.id='+element.id);
	event.stopPropagation();
	event.preventDefault();
}

function fxf_eh_dragLeave(event)
{
	var element=$(Event.element(event));
	if(!element || !element.id)
		return;
//fxf_fn_taskMsg('fxf_eh_dragLeave: element.id='+element.id);
	event.stopPropagation();
	event.preventDefault();
}

function fxf_eh_dragEnd(event)
{
	var element=$(Event.element(event));
	if(!element || !element.id)
		return;
//fxf_fn_taskMsg('fxf_eh_dragEnd: element.id='+element.id);
	event.stopPropagation();
	event.preventDefault();
}

function fxf_eh_drop(event)
{
	var element=$(Event.element(event));
	if(!element || !element.id)
		return;
//fxf_fn_taskMsg('fxf_eh_drop: element.id='+element.id);
	event.stopPropagation();
	event.preventDefault();

	if(element.id == 'uploader_zone')
	{
		oID.oUploader.uploader_select.value='';
		var files=event.dataTransfer.files;

		// Multiple selection, but only one allowed?
		if(!oID.oUploader.multi && (files.length > 1))
		{
			if(oFXP.lang == 1)
				var msg='Es darf nur eine einzige Datei upgeloadet werden!';
			else
				var msg='Only one file can be uploaded!';
			fxf_fn_showMessage('010', msg);
		}
		else
		{
			for(var i=0; i<files.length; i++)
			{
				var add=true;
				for(var f=0; f<oID.oUploader.filelist.length; f++)
				{
					if(oID.oUploader.filelist[f].name == files[i].name)
					{
						add=false;
						break;
					}
				}
				if(add)
					oID.oUploader.filelist.push(files[i]);
			}

			oID.oUploader.totalsize=0;;

			var msg='files.length='+files.length+', oID.oUploader.filelist.length='+oID.oUploader.filelist.length+'\n\n';
			for(f=0; f<oID.oUploader.filelist.length; f++)
			{
				msg += f+': '+oID.oUploader.filelist[f].name+' (type='+oID.oUploader.filelist[f].type+', size='+oID.oUploader.filelist[f].size+' bytes)\n';
				oID.oUploader.totalsize += oID.oUploader.filelist[f].size;
			}
			msg += '\n-- Total size='+oID.oUploader.totalsize+' bytes';
//alert(msg);

			fxf_fn_uploadFile('');
		}
	}
}

function fxf_eh_help(event)
{
	Event.stop(event);
}

function fxf_eh_keyPressed(event)
{
	keyEvent=event;
	if(!aKeys)
	{
		Event.stop(event);
		return false;
	}

}

function fxf_eh_keyDown(event)
{
	keyEvent=event;
	fxf_fn_setPageStatus('red');
	if(!aKeys)
	{
		Event.stop(event);
		return false;
	}

	if(!emode)
		cmd='K';
	fxf_fn_dispMenu(event,0);

	var kc=event.keyCode;
	if(kc == 116)	// F5 = Reload
	{
		fxf_fn_reload();
		Event.stop(event);
		return false;
	}

	// Leave function if text popup is displayed
	if(oID.txtpop.style.display != 'none')
		return true;

	var ks='';
	if(kc == 173)
		ks='-';
	else if(((kc > 47) && (kc < 58)) || ((kc > 64) && (kc < 91)))	// 0-9 or A-Z
		ks=String.fromCharCode(kc);
//fxf_fn_taskMsg('fxf_eh_keyDown: keyCode='+kc+', keyString='+ks);
//alert('fxf_eh_keyDown: keyCode='+kc+', keyString='+ks);return;

	if(oFXP.tr == 189)
		var element=fxf_fn_getField($(Event.element(event)),true);
	else
		var element=fxf_fn_getField($(Event.element(event)),false);
	if((!element || !element.id) && (oID.selpop.style.display != 'none'))
		element=fxf_fn_getField(oID.selpop,false);
	var node=element.nodeName.toLowerCase();

	var ei='';
	if(element && element.id)
		ei=element.id;

	// No key commands if reload info popup
	if((oID.iainfo.style.display != 'none') && (imode == 'rld'))
	{
		Event.stop(event);
		return false;
	}

	// No key commands (except Return or Esc) if info popup in matrix
	else if((oID.iainfo.style.display != 'none') && ei.length && (oFXP.tr == 189) && (kc != 13) && (kc != 27) && (imode == 'inf'))
	{
		if(ei.substr(0,6) != 'rctxt_')
		{
			Event.stop(event);
			return false;
		}
	}

	// No backspace, except in active text fields
	else if((kc == 8) && (!element || !element.type || !element.type.length || ((element.type.substr(0,4) != 'text') && (element.type.substr(0,4) != 'pass')) || element.readOnly || element.disabled))
	{
		Event.stop(event);
		return false;
	}

	if(kc == 115)		// F4: Settings
		fxf_fn_setPopup(true);
	else if(kc == 112)	// F1: Help
	{
		fxf_fn_hlpPopup(true);
		Event.stop(event);
		return false;
	}
	else if((kc == 119) || (kc == 120))	// F8: Workflow + F9: Projects
	{
		var ww=parseInt(oID.iworkflow.style.width);
		var wl=parseInt(oID.iworkflow.style.left);
		var pw=parseInt(oID.iprojects.style.width);
		var pl=parseInt(oID.iprojects.style.left);
		if((wl >= -ww) && (pl >= -pw))
		{
			if(kc == 119)
			{
				if(wl >= 0)
					fxf_fn_shrinkWorkflow(0.2,-1);
				else
					fxf_fn_growWorkflow(0.2);
			}
			else if(pl >= 0)
				fxf_fn_shrinkProjects(0.2,-1);
			else
				fxf_fn_growProjects(0.2,'','','');
		}
	}
	else if(tSet.key_block && (ei != 'fxsrcinput') && (ei != 'pjsmt_src'))	// Block all keys?
	{
		Event.stop(event);
		return false;
	}
	else if(event.ctrlKey && (kc == 83) && (oFXP.tr != 189))	// CTRL+S (except in matrix)
	{
		var btn=fxf_fn_simClick(element,false);
		if(btn && (btn.style.display != 'none'))
			fxf_fn_fxSubmitButton(btn);
		Event.stop(event);
		return false;
	}
	else
	{
		if((oFXP.tr == 189) && oID.sdg_container && !gSet.wfsl && !gSet.prjsl && (oID.iainfo.style.display == 'none') && (oID.searchpop.style.display == 'none') && (oID.selpop.style.display == 'none') && (!oID.hrpool || oID.hrpool.style.display == 'none'))
		{
			if(!mcheck)
				fxf_fn_showStatusMessage(0);

			oID.saction.style.display='none';
			oID.gaction.style.display='none';
			soi='';
			soii=-1;
			moi='';
			moii=-1;

			var dmode=-2;
			if(element && element.id)
			{
				if(element.id == 'pprefix')
					dmode=-1;
				else if(mcolumns.length || rcolumns.length)
				{
					var ei=element.id;
					var bp=ei.indexOf('[');
					if(bp > 0)
						ei=ei.substr(0,bp);
					dmode=fxf_fn_getKeyFromColumn(ei);
					if(dmode < 0)
						dmode=fxf_fn_getKeyFromRColumn(ei);
					if(dmode < 0)
						dmode=-2;
				}
//alert('ei='+ei+', dmode='+dmode);
			}
//oID.fxtaskl.innerHTML='emode='+emode+', dmode='+dmode+', kc='+kc;

			var rights=0;
			if((pAct >= 0) && (pAct < pArray.length))
				rights=pArray[pAct].rights;
			var prights=rights;
			if(!pAct || !prights)
				prights=pArray[0].rights;

			if(!emode && (dmode < -1))
			{
				if(((kc == 13) || (kc == 84)) && (prights & 4))	// Return or T
				{
					var pc=fxf_fn_getNextValidProjectKey();
					if((pc < 0) || (pArray[pc].open == 'fo'))
					{
						if(event.shiftKey)
							fxf_fn_addProject(pc,'P');
						else
							fxf_fn_addProject(pc,'T');
					}
				}
				else if((kc == 80) && (prights & 4))	// P
				{
					var pc=fxf_fn_getNextValidProjectKey();
					fxf_fn_addProject(pc,'P');
				}
				else if(kc == 83)	// S
				{
					if(event.ctrlKey)
					{
						var sb=fxf_fn_getField($('mtx_save'),false);
						fxf_fn_fxSubmitButton(sb);
						Event.stop(event);
						return false;
					}
					else if(rights & 8)
						fxf_fn_switchProjectType(pAct,false);
				}
				else if((kc == 37) && (rights & 8))	// Crsr Left
				{
					var ett=fxf_fn_checkDecLevel(pAct);
					if(!ett.length)	// Decrease level
						fxf_fn_newLevel(-1, pArray[pAct].uid);
				}
				else if((kc == 39) && (rights & 8))	// Crsr Right
				{
					var ett=fxf_fn_checkIncLevel(pAct);
					if(!ett.length)	// Increase level
						fxf_fn_newLevel(1, pArray[pAct].uid);
				}
				else if(kc == 38)	// Crsr Up
				{
					if(pAct >= 0)
						pAct=fxf_fn_selPrevObject('P', pAct);
					else if(dAct >= 0)
						dAct=fxf_fn_selPrevObject('D', dAct);
					fxf_fn_markObject(true);
				}
				else if(kc == 40)	// Crsr Down
				{
					if((pAct >= 0) && (pAct < pArray.length))
						pAct=fxf_fn_selNextObject('P', pAct);
					else if((dAct >= 0) && (dAct < dArray.length))
						dAct=fxf_fn_selNextObject('D', dAct);
					fxf_fn_markObject(true);
				}
				else if((kc == 107) || (kc == 171) || (kc == 187))	// +
				{
					if((pAct >= 0) && (pArray[pAct].open == 'fc'))
						fxf_fn_folderProject(1, pArray[pAct].uid, true);
					fxf_fn_markObject(true);
				}
				else if((kc == 109) || (kc == 173) || (kc == 189))	// -
				{
					if((pAct >= 0) && (pArray[pAct].open == 'fo'))
						fxf_fn_folderProject(-1, pArray[pAct].uid, true);
					fxf_fn_markObject(true);
				}
				else if(kc == 46)	// Del
				{
					if(rights & 16)	// Project/Milestone
					{
						if(pAct != 0)
						{
							var ett=fxf_fn_checkDel(pAct);
							if(!ett.length)
								fxf_fn_delProject(pAct, true);
						}
					}
					else if((dAct >= 0) && (dAct < dArray.length))				// Dependency
						fxf_fn_delDependency(dAct, true);
				}
				else if((kc == 65) || (kc == 78))	// A or N
				{
					if(rights & 8)	// Project
					{
						if(!event.shiftKey)						// ...add next dependency
							fxf_fn_addNextDependency(pAct);
						else									// ...delete next dependencies
							fxf_fn_delDependencies(pAct, 'F');
					}
				}
				else if((kc == 66) || (kc == 86))	// B or V
				{
					if(rights & 8)	// Project
					{
						if(!event.shiftKey)						// ...add previous dependency
							fxf_fn_addPreviousDependency(pAct);
						else									// ...delete previous dependencies
							fxf_fn_delDependencies(pAct, 'S');
					}
				}
				else if((kc == 68) || (kc == 76))	// D or L
				{
					if(rights & 16)	// Project
					{
						if(event.shiftKey)
							fxf_fn_delDependencies(pAct, '');	// ...delete all dependencies
					}
				}
				else if(kc == 77)	// M
				{
					if(rights)	// Project
					{
						var mc=fxf_fn_countMilestones(pAct);
						if(!event.shiftKey && (mc >= 0) && (rights & 4))		// ...add milestone
							fxf_fn_addProject(pAct, 'M');
						else if(event.shiftKey && (mc > 0) && (rights & 16))	// ...delete milestones
						{
//alert('fxf_fn_delMilestones('+pAct+');');
							fxf_fn_delMilestones(pAct);
						}
					}
				}
				else if(kc == 90)	// Z
				{
					// Undo
					if(!event.shiftKey && event.ctrlKey)
						fxf_fn_undo(true);
					// Redo
					if(event.shiftKey && event.ctrlKey)
						fxf_fn_redo(true);
				}
				else if(kc == 113)	// F2
				{
					if((rights & 8) && fxf_fn_checkRename(pAct))	// Rename project
					{
						if(slarge && ((mka == 'S') || !glarge))
							fxf_fn_editText($('st'+pArray[pAct].uid));
						else if(glarge && ((mka != 'S') || !slarge))
						{
							if(pArray[pAct].type == 'M')
								fxf_fn_editText($('nm'+pArray[pAct].uid));
							else
								fxf_fn_editText($('nt'+pArray[pAct].uid));
						}
					}
				}
				else if((kc > 48) && (kc < 53))	// 1-4
				{
					if((rights & 8) && (pArray[pAct].type == 'M'))	// Switch milestone type
					{
						if(kc == 49)		// 1: Start
							fxf_fn_switchMilestoneType(pAct, -1);
						else if(kc == 50)	// 2: End
							fxf_fn_switchMilestoneType(pAct, 1);
					}
					else if((dAct >= 0) && (dAct < dArray.length))	// Change dependency type
					{
						var ka=fxf_fn_getKeyFromID('P', dArray[dAct].pida);
						var kb=fxf_fn_getKeyFromID('P', dArray[dAct].pidb);
						if(fxf_fn_checkDependency(ka, fxf_fn_getText(kc-13).substr(1,2), kb) != 0)
							fxf_fn_changeDependency(dAct, fxf_fn_getText(kc-13).substr(1,2));
					}
				}
				else if(kc == 48)	// 0
				{
					if((rights & 8) && (pArray[pAct].type == 'M'))	// Switch milestone type: Fix
						fxf_fn_switchMilestoneType(pAct, 0);
					else if((dAct >= 0) && (dAct < dArray.length))	// Switch dependency
					{
						var ka=fxf_fn_getKeyFromID('P', dArray[dAct].pida);
						var kb=fxf_fn_getKeyFromID('P', dArray[dAct].pidb);
						if(fxf_fn_checkDependency(kb, dArray[dAct].type.substr(1,1)+dArray[dAct].type.substr(0,1), ka) != 0)
							fxf_fn_switchDependency(dAct);
					}
				}
				else if(kc == 27)	// Esc
				{
					if(kesc == -1)
					{
						kesc=0;
						if(mcheck == 1)
							fxf_dh_actStop(event);
						else if(mcheck > 1)
							fxf_dh_depStop(event);
					}
				}
			}
			else if(dmode < -1)
			{
				if(kc == 9)			// Tab
					fxf_fn_editStop(event,true);
				else if(kc == 13)	// Return
				{
					if(event.shiftKey || (cmd == 'M'))
						fxf_fn_editStop(event,true);
					else
						fxf_fn_editStop(event,false);
				}
				else if(kc == 27)	// Esc
					fxf_fn_editStop(event,false);
			}
			else if(dmode >= -1)
			{
//oID.fxtaskl.innerHTML='emode='+emode+', dmode='+dmode+', kc='+kc+' - oldValue='+oVar.fxv+', newValue='+element.fxv;
				if((kc == 13) || (kc == 9))
				{
					element.kc=kc;
					element.sdir=1;
					if(event.shiftKey)
						element.sdir=-1;
					if(dmode == -1)
						element.sdir=0;
					fEl=element;
					if(oVar.fxv != element.fxv)
						fxf_fn_saveElement.delay(0.05, element);
					else
						fxf_fn_focusNextElement.delay(0.05);
					Event.stop(event);
					return false;
				}
				else if((dmode == -1) && (kc == 32))	// No Space for Prefix allowed
				{
					Event.stop(event);
					return false;
				}
				else if(kc == 27)	// Esc
				{
					fxf_fn_unfocusText();
					Event.stop(event);
					return false;
				}
			}
		}
		else
		{
//fxf_fn_taskMsg('['+kc+'] - id='+element.id+', fxf='+element.fxf);
			if((element.id == 'selpop') && oVar.lastSelElement)
			{
				element=oVar.lastSelElement;
//fxf_fn_taskMsg('+ - id='+element.id+', fxf='+element.fxf);
			}
			if(kc == 9)			// Tab
			{
				if((element.fxf == 'sl') || (element.fxf == 'sm'))
					fxf_fn_unfocus(true,true,null);
			}
			else if(kc == 13) // Return
			{
				if((oFXP.tr == 110) && (element.id == 'dms_ren_nam'))
					fxf_fn_dmsAction('ren', -1, '');
				else if((oFXP.tr == 110) && (element.id == 'dms_newdirname'))
					fxf_fn_dmsAction('fls', -1, '');
				else if((oFXP.tr == 189) && oID.hrpool && (element.id.length == 7) && (element.id.substr(0,2) == 'hr') && (element.id.substr(4,3) == 'ftx'))
					fxf_fn_hrToggleFilter(element.id.substr(2,2));
				else if(element.id.substr(0,5) == 'fxsrc')
					fxf_fn_searchPopup(element);
				else if(element.id == 'pjsmt_src')
					fxf_fn_execPJSMControl('src');
				else if(element.id == 'ssearchinput')
					fxf_fn_dmsSearch.delay(oFXP.sdelay, null,false);
				else if(element.id == 'selpopsearchtext')
					fxf_fn_searchSelPopup.delay(oFXP.sdelay, 1969);
				else if(element.fxf && (element.fxf == 'sl'))
				{
					element.focus();
					fxf_eh_mouseDown(event,fxf_fn_getField(element,false));
				}
				else if(node != 'textarea')
				{
					if(oID.iainfo.style.display != 'none')
					{
						// Simulate click on OK button on PP/SP-Popup
						var cb=$('ppspok');
						if(cb)
							fxf_fn_ppspClose(true);
					}
					else if(oFXP.tr != 36)
					{
						var btn=fxf_fn_simClick(element,true);
						if(btn)
							fxf_fn_fxSubmitButton(btn);
						Event.stop(event);
						return false;
					}
				}
			}
			else if(kc == 27)	// Esc
			{
				if(oID.selpop.style.display != 'none')
				{
					fxf_fn_unfocus(true,true,null);
					element.focus();
				}
				if((oID.iainfo.style.display != 'none') || (oID.iact.style.display != 'none'))
					fxf_fn_fxLinkClose();

				if(oVar.lastTxtElement && (oID.txtpop.style.display != 'none'))
					fxf_fn_textPopup(-1,oVar.lastTxtElement);
			}
			else if((element.id == 'pprefix') && (kc == 32))	// No spaces allowed in project prefix
			{
				Event.stop(event);
				return false;
			}
			else if(element.fxf && (element.fxf == 'sl') && (ks.length || (kc == 32) || (kc == 37) || (kc == 38) || (kc == 39) || (kc == 40)))
			{
				var sc=false;
				Event.stop(event);
				if(oID.selpop.style.display == 'none')
				{
					element.focus();
					fxf_eh_mouseDown(event,fxf_fn_getField(element,false));
					sc=true;
				}
				var entrysel=$$('[class="sentrysel"]');
//alert('ks='+ks+', entrysel='+entrysel+', entrysel.length='+entrysel.length);
				if(entrysel.length)
				{
					var oelement=entrysel[0];
					var nelement=null;
					var psiblings=oelement.previousSiblings();
					var nsiblings=oelement.nextSiblings();
					var ep=psiblings.length;
					if(ks.length)
					{
						var np=ep;
						var vc='';
						if(nsiblings.length)
						{
							for(var s=0; s<nsiblings.length; s++)
							{
								vc=fxf_fn_getFirstChar(nsiblings[s].innerHTML,true);
								np++;
//alert('s='+s+', ep='+ep+', id='+nsiblings[s].id+', svalue='+nsiblings[s].attributes['svalue'].value+', innerHTML='+nsiblings[s].innerHTML+' -- np='+np+', vc='+vc);
								if(vc == ks)
								{
									nelement=nsiblings[s];
									ep=np;
//alert('FOUND: ep='+ep+', id='+nelement.id+', svalue='+nelement.attributes['svalue'].value+', innerHTML='+nelement.innerHTML);
									break;
								}
							}
						}
						if(!nelement && psiblings.length)
						{
							np=-1;
							for(var s=psiblings.length-1; s>=0; s--)
							{
								vc=fxf_fn_getFirstChar(psiblings[s].innerHTML,true);
								np++;
//alert('s='+s+', ep='+ep+', id='+psiblings[s].id+', svalue='+psiblings[s].attributes['svalue'].value+', innerHTML='+psiblings[s].innerHTML+' -- np='+np+', vc='+vc);
								if(vc == ks)
								{
									nelement=psiblings[s];
									ep=np;
//alert('FOUND: ep='+ep+', id='+nelement.id+', svalue='+nelement.attributes['svalue'].value+', innerHTML='+nelement.innerHTML);
									break;
								}
							}
						}
					}
					else if((kc == 39) || (kc == 40))
					{
						if(nsiblings.length)
						{
							if(kc == 40)
							{
								nelement=nsiblings[0];
								ep++;
							}
							else
							{
								nelement=nsiblings[nsiblings.length-1];
								ep += nsiblings.length;
							}
						}
					}
					else if(psiblings.length)
					{
						if(kc == 38)
						{
							nelement=psiblings[0];
							ep--;
						}
						else
						{
							nelement=psiblings[psiblings.length-1];
							ep=0;
						}
					}
					if(nelement)
					{
						oelement.className='sentry';
						nelement.className='sentrysel';

						// Scroll to selected entry
						var so=fxf_fn_getBCR(nelement);
						var lh=so.bottom-so.top;
						oID.selpop.scrollTop=ep*lh;

						var cn3=fxf_fn_getSelType(oVar.lastSelElement);
						var bg=nelement.style.background;
						oVar.lastSelElement.innerHTML=fxf_fn_convertHTMLEntities(nelement.innerHTML,true)+'<div class="'+cn3+'slc" style="cursor:pointer;"><span class="'+cn3+'sli"></span></div>';
						oVar.lastSelElement.attributes['svalue'].value=nelement.attributes['svalue'].value;
						oVar.lastSelElement.fxv=element.attributes['svalue'].value;
						oVar.lastSelElement.style.background=bg;
						oVar.lel=oVar.lastSelElement;
						oVar.fxv=null;
					}
				}
				if(sc && (oID.selpop.style.display != 'none'))
				{
					fxf_fn_unfocus(true,true,null);
					element.focus();
				}
				return false;
			}
			else if(element.fxf && (element.fxf == 'sm') && (ks.length || (kc == 32) || (kc == 37) || (kc == 38) || (kc == 39) || (kc == 40)))
			{
				Event.stop(event);
				var entrysel=element.select('[class="msentrysel"]');
				if(entrysel.length)
				{
					var oelement=entrysel[0];
					var nelement=null;
					var psiblings=oelement.previousSiblings();
					var nsiblings=oelement.nextSiblings();
					var ep=psiblings.length;
					if(ks.length)
					{
						var np=ep;
						var vc='';
						if(nsiblings.length)
						{
							for(var s=0; s<nsiblings.length; s++)
							{
								vc=fxf_fn_getFirstChar(nsiblings[s].innerHTML,true);
								np++;
//alert('s='+s+', ep='+ep+', id='+nsiblings[s].id+', svalue='+nsiblings[s].attributes['svalue'].value+', innerHTML='+nsiblings[s].innerHTML+' -- np='+np+', vc='+vc);
								if(vc == ks)
								{
									nelement=nsiblings[s];
									ep=np;
//alert('FOUND: ep='+ep+', id='+nelement.id+', svalue='+nelement.attributes['svalue'].value+', innerHTML='+nelement.innerHTML);
									break;
								}
							}
						}
						if(!nelement && psiblings.length)
						{
							np=-1;
							for(var s=psiblings.length-1; s>=0; s--)
							{
								vc=fxf_fn_getFirstChar(psiblings[s].innerHTML,true);
								np++;
//alert('s='+s+', ep='+ep+', id='+psiblings[s].id+', svalue='+psiblings[s].attributes['svalue'].value+', innerHTML='+psiblings[s].innerHTML+' -- np='+np+', vc='+vc);
								if(vc == ks)
								{
									nelement=psiblings[s];
									ep=np;
//alert('FOUND: ep='+ep+', id='+nelement.id+', svalue='+nelement.attributes['svalue'].value+', innerHTML='+nelement.innerHTML);
									break;
								}
							}
						}
					}
					else if((kc == 39) || (kc == 40))
					{
						if(nsiblings.length)
						{
							if(kc == 40)
							{
								nelement=nsiblings[0];
								ep++;
							}
							else
							{
								nelement=nsiblings[nsiblings.length-1];
								ep += nsiblings.length;
							}
						}
					}
					else if(psiblings.length)
					{
						if(kc == 38)
						{
							nelement=psiblings[0];
							ep--;
						}
						else
						{
							nelement=psiblings[psiblings.length-1];
							ep=0;
						}
					}
					if(nelement)
					{
						for(var e=0; e<entrysel.length; e++)
							entrysel[e].className='msentry';
						nelement.className='msentrysel';

						// Scroll to selected entry
						var so=fxf_fn_getBCR(nelement);
						var lh=so.bottom-so.top;
						element.scrollTop=ep*lh;

						element.attributes['svalue'].value=nelement.attributes['svalue'].value;
						element.fxv=element.attributes['svalue'].value;
						oVar.lel=element;
						oVar.fxv=null;
					}
				}
				return false;
			}
			else if((oFXP.tr == 110) && (kc == 113))	// F2:	DMS - Rename directory or document
			{
				var dabi=$('dabi_ren');
				if(dabi && (dabi.className == 'ticonb'))
					fxf_fn_dmsRename();
			}
			else if(element.id.substr(0,12) == 'selpopsearch')
			{
				fxf_fn_searchSelPopup.delay(oFXP.sdelay, kc);
				oVar.selsearch=true;
			}
			else if((oFXP.tr == 189) && oID.hrpool && (element.id.length == 7) && (element.id.substr(0,2) == 'hr') && (element.id.substr(4,3) == 'ftx'))
				fxf_fn_openHRPool.delay(oFXP.sdelay);
		}
	}
}

function fxf_eh_keyUp(event)
{
	keyEvent=event;
	if(!aKeys)
	{
		Event.stop(event);
		return false;
	}

	if(!emode)
		cmd='K';

	var kc=event.keyCode;
//fxf_fn_taskMsg('fxf_eh_keyUp: keyCode='+kc);
//alert('fxf_eh_keyUp: keyCode='+kc);

	if((oFXP.tr == 189) && oID.sdg_container && (oID.iainfo.style.display == 'none'))
	{
		fxf_fn_dispMenu(event,0);
		if(!mcheck)
			fxf_fn_showStatusMessage(0);
	}

	Event.stop(event);
	return false;
}

function fxf_eh_mouseMove(event)
{
	// Get mouse position
	if(event.pageX != null)
	{
		mcx=event.pageX;
		mcy=event.pageY;
	}
	else
	{
		mcx=0;
		mcy=0;
		if(event.clientX != null)
		{
			mcx=event.clientX;
			mcy=event.clientY;
		}
		mcx=mcx + (document.documentElement.scrollLeft ? document.documentElement.scrollLeft : document.body.scrollLeft);
		mcy=mcy + (document.documentElement.scrollTop ? document.documentElement.scrollTop : document.body.scrollTop);
	}
//fxf_fn_taskMsg('fxf_eh_mouseMove: mcx='+mcx+', mcy='+mcy+' -- dim.wd.top='+dim.wd.top+', dim.wd.bottom='+dim.wd.bottom+', dim.wd.height='+dim.wd.height);

if(debug && oDebug && oDebug.fxdebugm) oDebug.fxdebugm.innerHTML='mcx:'+mcx+', mcy:'+mcy;

	// Workflow and project slider
	var wso=1.0 - Math.max(0,Math.min(300,mcx))/600;
	if(dim && dim.wd && dim.wd.height)
	{
		var wsh=Math.floor((dim.wd.height-64)/2.4);
		if((mcx > 20) || (mcy < dim.wd.top) || (mcy > dim.wd.bottom))
			wsh=32;
	}
	else
		var wsh=32;
	if(!gSet.prjsl && oID.iprojects)
	{
		oID.iprojects.setOpacity(wso);
		oID.iprojectsa.style.height=wsh+'px';
		oID.iprojectsat.style.top=Math.max(32,Math.floor((wsh-oVar.dimiprojectsat.height)/2 + 8))+'px';
	}
	if(!gSet.wfsl && oID.iworkflow)
	{
		oID.iworkflow.setOpacity(wso);
		oID.iworkflowa.style.height=wsh+'px';
		oID.iworkflowat.style.bottom=Math.max(32,Math.floor((wsh-oVar.dimiworkflowat.height)/2 + 8))+'px';
	}

	fxf_fn_sAction(false);
}

function fxf_eh_focus(event)
{
	if(oVar.lel)
		fxf_fn_saveLastElement(oVar.lel);

	var element=fxf_fn_getField($(Event.element(event)),true);
//fxf_fn_taskMsg('fxf_eh_focus: element.id='+element.id);
	oVar.lel=null;
	oVar.fxv=null;
	if(element && element.id && (element.id == 'pjsmt_src'))
		fxf_fn_searchPJSM(true);
	else if(element && element.id && (element.id == 'ssearchinput'))
		fxf_fn_dmsSearch(element,true);
	else if((element.fxv != null) && element.fxf.length && (element.fxf != 'fx'))
	{
		oVar.lel=element;
		oVar.fxv=element.fxv;
fxf_fn_writeDebug('log+', '<b style="color:#00cc11;">fxf_eh_focus</b>:<br />id='+element.id+', name='+element.name+', type='+element.type+'<br />-&gt;uid='+element.uid+', fxf='+element.fxf+', fxv='+element.fxv);
	}
}

function fxf_eh_blur(event)
{
//alert('fxf_eh_blur() - call fxf_fn_saveLastElement()');
	var element=fxf_fn_getField($(Event.element(event)),true);
//fxf_fn_taskMsg('fxf_eh_blur: element.id='+element.id);
	if(element && element.id && (element.id == 'pjsmt_src'))
		fxf_fn_searchPJSM(false);
	else if(element && element.id && (element.id == 'ssearchinput'))
		fxf_fn_dmsSearch(element,false);
	else if(Prototype.Browser.IE && (Prototype.BrowserFeatures['Version'] < 9))
	{
		oFXP.addchg=true;
		fxf_fn_saveLastElement($(Event.element(event)));
	}
}

function fxf_eh_change(event)
{
	if(prohibitChanges)
	{
		Event.stop(event);
		return false;
	}

	var element=fxf_fn_getField($(Event.element(event)),true);
	var ei='';
	if(element && element.id)
		ei=element.id;
//fxf_fn_taskMsg('fxf_eh_change: element.id='+element.id);
	ei2=ei.substr(0,2);
	ei4=ei.substr(0,4);
	ei6=ei.substr(0,6);
fxf_fn_writeDebug('log+', '<b style="color:#0011cc;">fxf_eh_change</b>:<br />id='+element.id+', name='+element.name+', type='+element.type+'<br />-&gt;uid='+element.uid+', fxf='+element.fxf);
	if(ei == 'pjsmt_src')
		fxf_fn_execPJSMControl('src');
	else if((oFXP.tr == 7) || (oFXP.tr == 10))	// Employee or Contractor Contract Data
	{
		oFXP.addchg=true;
		if((ei == 'WochenArbeitStd') || (ei == 'Arbeitsbeginn') || (ei == 'Pause') || (ei == 'Montag') || (ei == 'Dienstag') || (ei == 'Mittwoch') || (ei == 'Donnerstag') || (ei == 'Freitag') || (ei == 'Samstag') || (ei == 'Sonntag'))
			fxf_fn_updateContractData(element);
		else
			fxf_fn_saveElement(element);
	}
	else if((oFXP.tr == 11) && ((ei2 == 'gg') || (ei2 == 'cg')))
	{
		var ga=ei.substr(2).split('_');
		var gc=parseInt(ga[0]);
		var cc=0;
		if(ga.length > 1)
			cc=parseInt(ga[1]);
		var c=element.value;
		if(c.length)
		{
			c=parseInt(c);
			if(isNaN(c))
				c='';
		}
//alert('ga='+ga+'\n\ngc='+gc+', cc='+cc+', c='+c);
		fxf_fn_prjcatSetBG(gc,cc,c);
	}
	else if((oFXP.tr == 56) && ((ei4 == 'cb0_') || (ei4 == 'cb1_') || (ei4 == 'cb2_') | (ei4 == 'cb3_') || (ei4 == 'cb4_')))
	{
		var ci=parseInt(ei.substr(2,1));
		var io=ei.indexOf('[');
		var pid=ei.substr(4,io-4);
		var mc=ei.substr(io+1);
		var io=mc.indexOf('[');
		var ln=parseInt(mc.substr(io+1,mc.length-io-2));
		var mc=parseInt(mc.substr(0,io-1));
		fxf_fn_cbClick(ci,pid,mc,ln,-1);
		Event.stop(event);
		return false;
	}
	else if((oFXP.tr == 107) || (oFXP.tr == 110))
	{
		if(ei6 == 'nup_cb')
		{
			var id=parseInt(ei.substr(7));
			fxf_fn_dmsSelectFile(id, true);
		}
		else if((ei != 'txtpopta') && (ei6 != 'ssearc'))
		{
			oFXP.addchg=true;
			fxf_fn_dmsSave('change', ei);
		}
		Event.stop(event);
		return false;
	}
	else
	{
		oFXP.addchg=true;
		fxf_fn_saveElement(element);
	}
}

function fxf_fn_loadPTR(ntr)
{
	if(oFXP.ptr || (oFXP.tr == ntr))
		return;

	// Warn user if values have been edited and transaction is not in display mode
	if((oFXP.changed > 0) && (oFXP.action > 1) && (oFXP.tr > 0))
	{
		var msg=12;
		if(ntr == 223)
			msg=7;
		fxf_fn_question(fxf_fn_getMessage(msg,true), fxf_fn_getMessage(msg,true), [fxf_fn_getText(10), fxf_fn_getText(11)], ['fxf_fn_resetChanged();fxf_fn_loadPTR('+ntr+');', ''], 100);
		return;
	}

	oFXP.ptr=ntr;
	oFXP.ltr=oFXP.tr;
	oFXP.tr=ntr;
//alert('fxf_fn_loadPTR(ntr='+ntr+')\nooFXP.ptr='+oFXP.ptr+', oFXP.ltr='+oFXP.ltr+', FXP.tr='+oFXP.tr);

	if(ntr == 223)
		fxf_fn_clickFilter(true);

	fxf_fn_waitScreen('loading');
	fxf_fn_loadTR(oFXP.tr,'newptr');
}

function fxf_fn_displayPTR(data)
{
	if(oID.ptrans.style.display == 'none')
	{
		oID.ptmain.style.width='auto';
		oID.ptmain.style.height='auto';
		oID.ptmain.innerHTML='<input id="ptmainfocus" type="text" value="" style="border:0;background:transparent;position:absolute;left:0;top:-'+dim.sd.pheight+'px;width:0;height:0;">'+data;

		aEl=document.activeElement;

		fxf_fn_waitScreen('');
		oID.ptrans.style.display='';
		oID.ptcont.style.display='';
		$('ptmainfocus').focus();

		// Content dimensions
		var pew=oID.ptmain.clientWidth+24;
		var peh=oID.ptmain.clientHeight+48;

		// Scrolls offsets
		var qw=Math.min(pew, dim.sd.pwidth-160);
		var qh=Math.min(peh, dim.sd.pheight-198);
//alert('dim.sd.pwidth='+dim.sd.pwidth+', dim.sd.pheight='+dim.sd.pheight+' -- pew='+pew+', peh='+peh+' -- qw='+qw+', qh='+qh);

		oID.ptmain.style.width=qw+'px';
		oID.ptmain.style.height=qh+'px';
		oID.ptcont.style.left=(Math.floor(dim.sd.pwidth/2 - qw/2)-24)+'px';
		oID.ptcont.style.top=(Math.floor(dim.sd.pheight/2 - qh/2)-8)+'px';
	}
	else
		oID.ptmain.innerHTML=data;
}

function fxf_fn_closePTR()
{
	if(oFXP.tr == 223)
		fxf_fn_clickFilter(false);

	oFXP.ptr=0;
	oFXP.tr=oFXP.ltr;
//alert('fxf_fn_closePTR()\noFXP.ptr='+oFXP.ptr+', oFXP.ltr='+oFXP.ltr+', FXP.tr='+oFXP.tr);

	oID.ptrans.style.display='none';
	oID.ptcont.style.display='none';
	oID.ptmain.innerHTML='';

	fxf_fn_waitScreen('reloading');
	fxf_fn_loadTR.delay(oFXP.ldelay, oFXP.ltr,'PReload');
}

function fxf_fn_loadTR(ntr,uid)
{
	if(oFXP.ptr)
		fxf_fn_taskMsg('Loading &uarr;PF #'+ntr+' .');
	else
		fxf_fn_taskMsg('Loading PF #'+ntr+' .');
//alert('fxf_fn_loadTR(ntr='+ntr+', uid='+uid+')\nwindow.location='+window.location+'\nwindow.location.search='+window.location.search);
	if((ntr <= 0) && (window.location.search == '?autologin=1')) { var hURL=window.location.origin+window.location.pathname; var hTitle='fx-project Login'; var hState={'additionalInformation':'fx-project Login Page'}; window.history.replaceState(hState,hTitle,hURL); fxf_fn_loadTR.delay(oFXP.sdelay, 0,'Button_Login&frm=_fxform_login_3'); return; }

	fxf_fn_selPopup(false);

	if(oFXP.pwindow)
	{
		oFXP.pwindow.close();
		oFXP.pwindow=null;
	}

	if(oFXP.busy > 0.0)
	{
		fxf_fn_loadTR.delay(oFXP.sdelay, ntr,uid);
		return;
	}

	if(oVar.lel)
	{
		fxf_fn_saveLastElement(oVar.lel);
		fxf_fn_loadTR.delay(oFXP.sdelay, ntr,uid);
		return;
	}
	fxf_fn_taskMsg('+.');

	oFXP.addchg=false;
	if(gSet.wfsl >= 1)
		fxf_fn_shrinkWorkflow(0.15,-1);
	if(gSet.prjsl >= 1)
		fxf_fn_shrinkProjects(0.15,-1);

	if(oFXP.tr == 217)	// Form Design
		fxf_fn_cleanupEventListeners();

	var otr=oFXP.tr;

	ntr=ntr+'';
	if(!ntr.length)
		ntr=otr;
	ntr=parseInt(ntr);
//alert('fxf_fn_loadTR: otr='+otr+', ntr='+ntr+', uid='+uid);

	// Warn user by transaction change if values have been edited and transaction is not in display mode
	if((oFXP.changed > 0) && (oFXP.action > 1) && (otr > 0) && (ntr != otr))
	{
		fxf_fn_taskMsg('+?');
		fxf_fn_waitScreen('');
		fxf_fn_question(fxf_fn_getMessage(5,true), fxf_fn_getMessage(5,false), [fxf_fn_getText(10), fxf_fn_getText(11)], ['fxf_fn_resetChanged();fxf_fn_waitScreen(\'loading\');fxf_fn_loadTR('+ntr+',\''+uid+'\')', ''], 100);
		return;
	}
	else if((otr == 35) && tSet.oprofiles && tSet.oprofiles.length)	// Profile Names
	{
		var nprofiles=fxf_fn_getProfiles();
//alert('action='+oFXP.action+'\n\nOld Profiles: '+tSet.oprofiles+'\nNew Profiles: '+nprofiles);
		var dprf=0;
		if(nprofiles != tSet.oprofiles)
		{
			for(var s=0; s<nprofiles.length; s++)
			{
				if(((oFXP.action == 4) && (nprofiles.substr(s,1) == '1')) || ((nprofiles.substr(s, 1) != tSet.oprofiles.substr(s, 1)) && (nprofiles.substr(s, 1) == '0')))
					dprf++;
			}
		}
//alert('dprf='+dprf);
		if(dprf > 0)
		{
			fxf_fn_taskMsg('+?');
			fxf_fn_waitScreen('');
			if(oFXP.lang == 1)
			{
				var chead='Profilgruppe(n) deaktivieren?';
				var ctext='<b class="red">Wollen Sie wirklich '+dprf+' Profilgruppe(n) ';
				if(oFXP.action != 4)
					ctext += 'deaktivieren<br />';
				ctext += 'und alle dazugehörigen Rechte löschen?</b><br /><br /><br /><b>HINWEIS:</b><br /><br />Benutzer, welche diesen Profilgruppen zugeordnet sind, können anschliessend nicht mehr<br />auf diejenigen Programme zugreifen, welche in diesen Gruppen hinterlegt sind!';
			}
			else
			{
				var chead='Deactivate Profile Group(s)?';
				var ctext='<b class="red">Do you really want to ';
				if(oFXP.action != 4)
					ctext += 'deactivate '+dprf+' profile group(s)<br />and delete';
				else
					ctext += 'delete '+dprf+' profile group(s) and';
				ctext += ' all assigned rights?</b><br /><br /><br /><b>ATTENTION:</b><br /><br />Users assigned to these profile groups cannot access those<br />programs anymore, that are assigned to these groups!';
			}
			fxf_fn_question(chead, ctext, [fxf_fn_getText(10), fxf_fn_getText(11)], ['tSet.oprofiles=\'\';fxf_fn_waitScreen(\'saving\');fxf_fn_loadTR('+ntr+',\''+uid+'\')', ''], 100);
			return;
		}
	}
	else if((otr == 43) && (uid.substr(0,3) == 'MAZ') && tSet.ntasks && (tSet.ntasks > 1) && tSet.oassigned && tSet.oassigned.length)	// Assign Resources
	{
		var nassigned=fxf_fn_getAssignedResources();
		if(nassigned != tSet.oassigned)
		{
			var ares=0;
			var dres=0;
			for(var s=0; s<nassigned.length; s++)
			{
				if((tSet.oassigned.substr(s,1) == '1') && (nassigned.substr(s,1) == '0'))
					dres++;
				if(nassigned.substr(s,1) == '1')
					ares++;
			}
			fxf_fn_taskMsg('+?');
			fxf_fn_waitScreen('');
			if(oFXP.lang == 1)
			{
				var chead='Mitarbeiterzuordnung wirklich ändern?';
				var ctext='<b class="blue">Wollen Sie wirklich bei allen untergeordneten '+tSet.ntasks+' Aufgaben<br />';
				if(!ares)
					ctext += 'sämtliche Mitarbeiter (HR) entfernen';
				else
				{
					ctext += ares+' Mitarbeiter (HR) zuordnen';
					if(dres > 0)
						ctext += ' und '+dres+' entfernen';
				}
				ctext += '?</b><br /><br /><br /><b>HINWEIS:</b><br /><br />Alle vorhandenen bisherigen Zuordnungen in allen untergeordneten Aufgaben<br />werden hierbei aufgelöst und ersetzt durch die neue Zuordnung!';
			}
			else
			{
				var chead='Really Change Human Resource Assignments?';
				var ctext='<b class="blue">Do you really want to ';
				if(!ares)
					ctext += 'release all Human Resources (HR)';
				else
				{
					ctext += 'assign '+ares;
					if(dres > 0)
						ctext += ' and release '+dres;
					ctext += ' Human Resources (HR)';
				}
				ctext += '<br />in all '+tSet.ntasks+' lower-level tasks?</b><br /><br /><br /><b>ATTENTION:</b><br /><br />All existing assignments in all lower-level tasks will be released<br />and replaced with the new assignment!';
			}
			fxf_fn_question(chead, ctext, [fxf_fn_getText(10), fxf_fn_getText(11)], ['tSet.oassigned=\'\';fxf_fn_waitScreen(\'saving\');fxf_fn_loadTR('+ntr+',\''+uid+'\')', ''], 100);
			return;
		}
	}
	else if((otr == 223) && (uid.substr(0,15) == 'Button_Loeschen') && (uid.substr(uid.length-5,5) != '&dc=1'))	// Delete filter
	{
		fxf_fn_taskMsg('+?');
		fxf_fn_waitScreen('');
		// Delete Filter?
		fxf_fn_question(fxf_fn_getMessage(13,true), fxf_fn_getMessage(13,false), [fxf_fn_getText(10), fxf_fn_getText(11)], ['fxf_fn_waitScreen(\'deleting\');fxf_fn_loadTR('+ntr+',\''+uid+'&dc=1\')', ''], 100);
		return;
	}
	fxf_fn_taskMsg('+.');

	fxf_fn_resetChanged();
	var btime=new Date();

	if(oFXP.ptr)
	{
		oID.ptmessage.style.display='none';
		oID.ptbuttons.style.display='none';
	}
	else
	{
		oID.imessage.style.display='none';
		oID.fxbuttons.style.display='none';
	}

	var ouid=uid;
	if(uid.length)
		uid='&uid='+uid;

	if(!ntr)
		uid += '&swidth='+dim.sd.pwidth+'&sheight='+dim.sd.pheight;

	fxf_fn_drawMenuBorder('', ntr);

	var jse=0;
	if($('trjsa_'+fxf_fn_addLeadingZeros(ntr,3)))
		jse=1;

	var ssm='';
	if(scrshtmode == 1)
		ssm='&ssm=1';

	var url=fxf_fn_worker('tr','culang='+oFXP.lang+'&otr='+otr+'&ntr='+ntr+'&ptr='+oFXP.ptr+'&jse='+jse+ssm+uid+'&msl='+oFXP.mstructure.length);
//alert('url='+url);
	fxf_fn_writeDebug('log', '<b>loadTR</b>: otr='+otr+', oFXP.mstructure.length='+oFXP.mstructure.length+'<br />url='+url+'<hr>');
	fxf_fn_taskMsg('+.');

	new Ajax.Request(url,
	{
		method:'get', asynchronous:true,
		onSuccess: async function(transport)
		{
			fxf_fn_taskMsg('TR loaded');
			var ret=transport.responseText;
//fxf_fn_writeDebug('log', ret);
			var data=ret.split('*~*');
fxf_fn_writeDebug('log+', 'data.length='+data.length+'<hr>');
for(var i=0; i<data.length; i++)
fxf_fn_writeDebug('log+', 'data['+i+'].length='+data[i].length);
			fxf_fn_waitScreen('');
			var cresize=true;
			if(data.length > 3)
			{
				// Data
				if(oFXP.ptr)
					fxf_fn_displayPTR(data[0]);
				else
					oID.fxwork.innerHTML=data[0];
				var dmseid=$('dmseid');
				if(dmseid)
					fxf_fn_toogleMails();

				// Backup special variables
				var blang=oFXP.lang;
				if(!oSet.dateformat || !oSet.dateformat.length)
					var bdateformat='';
				else
					var bdateformat=oSet.dateformat;

				// Language
				var olang=oFXP.lang;
				var dtrlang=$('trlang');
				if(dtrlang)
				{
					var dtra=dtrlang.innerHTML.split(divstr[0]);
					if(dtra.length)
					{
						oFXP.lang=parseInt(dtra[0]);
						for(var d=1; d<dtra.length; d++)
						{
							var dtrit=dtra[d].split('|');
							var dtria=dtrit[0].split('^');	// Field id + possible variable attribute
							var dtri=$(dtria[0]);
							var dtrc=dtrit[1].substr(0,1);
							if(dtri)
							{
								if((dtria.length > 1) && dtri.attributes && dtri.attributes[dtria[1]])	// variable attribute
									dtri.attributes[dtria[1]].value=dtrit[1];
								else if((dtrc == '^') && dtri.attributes && dtri.attributes['tooltip'])	// tooltip
									dtri.attributes['tooltip'].value=dtrit[1].substr(1);
								else if(dtrc == '~')	// value
									dtri.value=dtrit[1].substr(1);
								else					// innerHTML
									dtri.innerHTML=dtrit[1];
							}
						}
					}
				}
				var djslang=$('jslang');
				if(djslang)
				{
					aTXT=djslang.innerHTML.split(divstr[0]);
//alert('aTXT: ('+aTXT.length+' entries)\n'+aTXT);
					fxf_fn_getText(5);
					fxf_fn_clock(true,false);
				}

				// Flag
				if(olang != oFXP.lang)
					oID.fxtaskli.src='GFX/flg_'+fxf_fn_addLeadingZeros(oFXP.lang,2)+'.png';

				// Enlarge
				if(($('esenl').style.display == 'none') && ($('esshr').style.display == 'none') && oFXP.lang)
				{
					$('esenl').style.display='';
					$('esshr').style.display='none';
					fxf_fn_resize();
				}

				// Default help file
				var dhf_frame=$('dhf_frame');
				if(oID.fxphoto_frame && oID.fxphoto_frame_href && dhf_frame)
				{
					var dfp=dhf_frame.innerHTML.indexOf('*');
					oID.fxphoto_frame.src=dhf_frame.innerHTML.substr(0,dfp);
					oID.fxphoto_frame.attributes['tooltip'].value='Take Frame Screenshot: '+dhf_frame.attributes['tooltip'].value;
					oID.fxphoto_frame_href.href=dhf_frame.innerHTML.substr(dfp+1)+'?'+Date.now();
					oID.fxphoto_frame_href.attributes['tooltip'].value='Open Frame Screenshot: '+dhf_frame.attributes['tooltip'].value;
				}

				// Common variables
				var doset=$('oset');
				if(doset)
				{
					oSet={};
					var soset=doset.innerHTML.split('|');
					doset.outerHTML='';
//alert('soset.length='+soset.length+'\n'+soset);
					if(soset.length)
					{
						for(var i=0;i<soset.length;i++)
						{
							var voset=soset[i].split('=');
							var type=voset[1].substr(0,1);
//alert(i+'('+type+'): voset=\n'+voset);
							if(type == 'A')		// Array
							{
								oSet[voset[0]]=voset[1].substr(1).split(',');
								for(var a=0;a<oSet[voset[0]].length;a++)
									oSet[voset[0]][a]=fxf_fn_setOSet('',oSet[voset[0]][a],'o');
//alert('Array: oSet['+voset[0]+'] ('+oSet[voset[0]].length+') =\n'+oSet[voset[0]]);
							}
							else
								fxf_fn_setOSet(voset[0],voset[1],'o');
						}
//alert('oSet.client='+oSet.client+'\noSet.user='+oSet.user+'\noSet.person='+oSet.person+'\noSet.pname='+oSet.pname+'\noSet.dateformat='+oSet.dateformat+'\noSet.timeformat='+oSet.timeformat+'\noSet.decimalpoint='+oSet.decimalpoint+'\noSet.decimallength='+oSet.decimallength+'\noSet.cunit='+oSet.cunit+'\noSet.unitselect='+oSet.unitselect+'\noSet.unitcalc='+oSet.unitcalc+'\noSet.unitlit='+oSet.unitlit);
						// Get currency character and length
						taconv.innerHTML=oSet.currency;
						oSet.currency_character=oID.taconv.value;
						oSet.currency_length=oSet.currency_character.length;
//alert('oSet.currency: '+oSet.currency+'\noSet.currency_character: '+oSet.currency_character+'\noSet.currency_length: '+oSet.currency_length);
					}

					// Unit settings
					if(oSet.cunit)
						oSet.cunit=fxf_fn_getUnit(oSet.cunit);
					if(oSet.punit)
						oSet.punit=fxf_fn_getUnit(oSet.punit);
//alert('oSet.cunit='+oSet.cunit+'\noSet.punit='+oSet.punit);

					// Global settings
//alert('oSet.sethelp='+oSet.sethelp+'\noSet.setalignworkday='+oSet.setalignworkday+'\noSet.setshowframes='+oSet.setshowframes+'\noSet.setshowlines='+oSet.setshowlines+'\noSet.setshowhr='+oSet.setshowhr+'\noSet.setcontnumbers='+oSet.setcontnumbers);
					gSet.filter=oSet.setfilter;
					gSet.help=oSet.sethelp;
					gSet.trmark=oSet.settrmark;
					gSet.ppsppopalways=oSet.setppsppopalways;
					gSet.alignworkday=oSet.setalignworkday;
					gSet.showframes=oSet.setshowframes;
					gSet.showlines=oSet.setshowlines;
					gSet.showhr=oSet.setshowhr;
					gSet.contnumbers=oSet.setcontnumbers;
					gSet.wfcolor=oSet.setwfcolor;
					gSet.wfairow=oSet.setwfairow;
					fxf_fn_taskMsg('+ - CVars set');

					fxf_fn_setFilter();
				}

				// Special variable(s) has/have changed?
				if((blang != oFXP.lang) || (oSet.dateformat && oSet.dateformat.length && (bdateformat != oSet.dateformat)))
					fxf_fn_clock(true,true);

				// Transaction variables
				tSet={};
				var dtset=$('tset');
				if(dtset)
				{
					var stset=dtset.innerHTML.split('|');
					dtset.outerHTML='';
//alert('stset.length='+stset.length+'\n'+stset);
					if(stset.length)
					{
						for(var i=0;i<stset.length;i++)
						{
							var vtset=stset[i].split('=');
							var type=vtset[1].substr(0,1);
//alert(i+'('+type+'): vtset=\n'+vtset);
							if(type == 'A')		// Array
							{
								tSet[vtset[0]]=vtset[1].substr(1).split(',');
								for(var a=0;a<tSet[vtset[0]].length;a++)
									tSet[vtset[0]][a]=fxf_fn_setOSet('',tSet[vtset[0]][a],'t');
//alert('Array: tSet['+vtset[0]+'] ('+tSet[vtset[0]].length+') =\n'+tSet[vtset[0]]);
							}
							else
								fxf_fn_setOSet(vtset[0],vtset[1],'t');
						}
						fxf_fn_taskMsg('+ - TVars set');
					}
				}

				tSet.key_block=false;
				var dkb=$('key_block');
				if(dkb)
					tSet.key_block=true;

				// Languages
				var langset=$('langset');
				if(langset)
				{
					var lshtml=langset.innerHTML;
					langset.outerHTML='';
					var lsdata=lshtml.split('|');
					var lar=[];
					if(lsdata.length)
					{
//alert('lsdata: length='+lsdata.length+'\n\n'+lsdata);
						for(var li=0; li<lsdata.length; li++)
						{
							var lidata=lsdata[li].split('_');
							lar[li]={'id':parseInt(lidata[0]), 'sign':lidata[1].substr(0,2).toUpperCase(), 'text':lidata[1]};
						}
					}
					fxf_fn_drawLanguage.delay(oFXP.mdelay, lar);
					fxf_fn_taskMsg('+ - Language set');
				}

				// Menu structure, entries and colors
				fxf_fn_setMenu('', ntr);

				// Filter
				var fltset=$('fltset');
				if(fltset)
				{
					var flthtml=fltset.innerHTML;
					fltset.outerHTML='';
					var fltdata=flthtml.split(divstr[2]);
					if(fltdata.length)
					{
						oID.fxfltsel.attributes['svalue'].value=fltdata[0];
						if(fltdata.length > 1)
							oID.fxfltsel.innerHTML=fltdata[1]+'<div class="fmfslc" style="cursor:pointer;"><span class="fmfsli"></span></div>';
						if((fltdata.length > 2) && fltdata[2].length)
							oID.fxfltsel.attributes['tooltip'].value='<b>'+fltdata[1]+'</b><hr size=1 /><br />'+fltdata[2]+'<br /><br />';
						else
							oID.fxfltsel.attributes['tooltip'].value='';

						var idt=$('sel_fxfltsel');
						if(idt && (fltdata.length > 3))
						{
							idt.innerHTML=fltdata[3];
						}
					}
					fxf_fn_taskMsg('+ - Filter set');
				}

				// Person info
				var taskpinfo=$('taskpinfo');
				if(taskpinfo)
				{
					var tpihtml=taskpinfo.innerHTML;
					taskpinfo.outerHTML='';
					var tpidata=tpihtml.split('|');
					if(tpidata.length)
					{
						oID.fxtaskp.innerHTML=tpidata[0];
						oID.fxtaskpp.innerHTML=tpidata[1];
					}
					fxf_fn_taskMsg('+ - User set');
				}

				// Buttons
				if(oFXP.ptr)
				{
					oID.ptbuttons.innerHTML=data[1];
					oID.ptbuttons.style.display='';
					oID.fxbuttons.innerHTML='';
					oID.fxbuttons.style.display='none';
				}
				else
				{
					oID.ptbuttons.innerHTML='';
					oID.ptbuttons.style.display='none';
					oID.fxbuttons.innerHTML=data[1];
					oID.fxbuttons.style.display='';
				}

				// List headers
				oFXP.listha=[];
				var lhids=$$('[id^="listheader"]');
//alert('['+lhids.length+']: lhids='+lhids);
				if(lhids.length)
				{
					for(var lhc=0; lhc<lhids.length; lhc++)
					{
						oFXP.listha[lhc]={};
						oFXP.listha[lhc].height=0;
						oFXP.listha[lhc].src=lhids[lhc];
						oFXP.listha[lhc].div=new Element('div', {id:'flistheader', style:'position:absolute;left:0;top:0;width:0;height:0;white-space:normal;display:none;'});
						oFXP.listha[lhc].div.innerHTML='<table width="100%" border="0" cellpadding="0" cellspacing="0"><tr id="flistheader'+lhc+'">'+lhids[lhc].innerHTML+'</tr></table>';
						oID.fxwork.insert(oFXP.listha[lhc].div);
					}
				}

				// Design
				var psdi=$('ps_design');
				if(psdi)
				{
					var ps_design=psdi.innerHTML;
					fxf_fn_setDesign(ps_design);
				}
				var tbdi=$('tb_design');
				if(tbdi)
				{
					oID.fxtaskdp.innerHTML=tbdi.innerHTML;
					var tp=tbdi.parentNode;
					tp.removeChild(tbdi);
				}

				// Action button counter
				var abc=0;

				// Status
				sdata=data[3].split('*#*');
//alert('sdata.length='+sdata.length+'\n\n'+sdata);
				if(sdata.length > 2)
				{
					stype=fxf_fn_setMessageStatus(sdata[0]);
					fxf_fn_taskMsg(sdata[1]);
					if(oFXP.ptr)
						oID.ptmessagem.innerHTML=sdata[2];
					else
						oID.imessagem.innerHTML=sdata[2];
					tdata=data[2].split('|');
//alert('tdata.length='+tdata.length+'\n\n'+tdata);
					if(tdata.length > 3)
					{
						oFXP.tr=parseInt(tdata[0]);
						if(oFXP.ptr)
						{
							oID.pttitle.innerHTML=tdata[3];
							oID.ptfooter.innerHTML=tdata[4];
						}
						else
						{
							oID.fxpath.innerHTML=tdata[2];
							oID.fxheader.innerHTML='<span id="fxheadert" class="fxheader">'+tdata[3]+'</span>';
							oID.fxfooter.innerHTML=tdata[4];
//alert('OK: otr='+otr+' - oFXP.tr='+oFXP.tr);

							// Actions
							oFXP.action=0;
							for(var a=0; a<4; a++)
							{
								var aa=parseInt(tdata[1].substr(a,1));
								if(aa < 1)
									$('actreg'+(a+1)).className='actregi';
								else
								{
									abc++;
									if(aa > 1)
									{
										$('actreg'+(a+1)).className='actrega';
										oFXP.action=a+1;
									}
									else
										$('actreg'+(a+1)).className='actreg';
								}
							}
							if(abc < 2)
								oID.fxtaction.style.display='none';
							else
								oID.fxtaction.style.display='';
						}
					}
					// ...OK
					var msg=0;
					if((stype == 0) || (stype == 1) || (sdata[2] == '?') || (sdata[2].length == 0))
					{
						if(tdata.length > 3)
						{
							// Messages
							if((sdata[2].length > 0) && (sdata[2] != '?'))
							{
								msg=1;
								fxf_fn_shrinkMessage(0.1,true);
							}
						}

						var jdct=parseInt(sdata[3]);
						if(jdct < 0)
						{
							var data='<div id="dtxpu" class="maskc" style="position:relative;padding:6px;white-space:normal;text-align:center;">';
							data += '<div class="bold" style="background:#ecb5bb;border:1px solid #e1001a;border-radius:6px;"><br />';
							if(oFXP.lang == 1)
								data += 'Achtung';
							else
								data += 'Attention';
							data += '!<br /><br /></div><br /><br />';
							if(oFXP.lang == 1)
								data += 'fx-project wurde gerade vom Administrator aktualisiert,<br />deshalb muss die komplette Seite neu geladen werden.';
							else
								data += 'fx-project has just been updated by the Administrator,<br />therefore the whole page needs to be reloaded.';
							data += '<br /><br /><br />';
							data += '<input class="fxfbs" type="submit" style="text-align:center;width:120px;" onclick="fxf_fn_reload();" value="&nbsp;';
							if(oFXP.lang == 1)
								data += 'Neu laden';
							else
								data += 'Reload';
							data += '&nbsp;" /><br /><br />';
							data += '</div>';
							imode='rld';
							fxf_fn_fxLinkDisplay(data);
						}
					}
					else
					{
//alert('FXP ERROR\n\n'+sdata[1]+'\n\n'+sdata[2]);
						msg=2;
						if(oID.fxframe.style.display == 'none')
						{
							fxf_fn_growMessage.delay(2.0, 0.5);
							fxf_fn_shrinkMessage.delay(4.0, 0.3,false);
						}
						else
						{
							fxf_fn_growMessage(0.5);
							fxf_fn_shrinkMessage.delay(3.0, 0.3,false);
						}
						if(!otr && (stype == 3) && ouid.length && (ouid.substr(0,12) == 'Button_Login') && $('mt44_kopf'))
							oFXP.tr=0;
					}
//alert('otr='+otr+', oFXP.tr='+oFXP.tr);

					// Units
					sunit={};
					if(oFXP.tr && oSet && oSet.unitlit && oSet.unitlit.length)
						fxf_fn_checkUnit();
					else
						oID.fxtasku.style.display='none';

					// Timespan format
					fxf_fn_formatTimespan('');

					var etime=new Date();
					var trdur=etime-btime;
					oID.fxtaski.innerHTML=(Math.round(trdur/10)/100)+'s';

					// Download
					var docdlidx=$('docdlidx');
					if(docdlidx)
					{
						var idx=parseInt(docdlidx.innerHTML);
						fxf_fn_docDownload.delay(oFXP.mdelay, idx);
					}

					// Use additional PF JavaScript?
					var ams=fxf_fn_addJavaScript(otr);
					if(ams)
						await fxf_fn_sleep(ams);

					// Display work area
					var esize=fxf_fn_setWorkMargin();

					if((gSet.wfsl >= 0) && oFXP.tr)
					{
						if(oID.iworkflow.style.display == 'none')
						{
							oID.iworkflow.style.display='';
							if(otr)
								fxf_fn_shrinkWorkflow.delay(1.0, 0.2,0);
						}
					}

					if((gSet.prjsl >= 0) && oFXP.tr)
					{
						if(oID.iprojects.style.display == 'none')
						{
							oID.iprojects.style.display='';
							if(otr)
								fxf_fn_shrinkProjects.delay(1.0, 0.2,0);
						}
					}
				}

				// Autoscroll
				var ast=0;
				var asl=0;
				var id_ts=$('topscroll');
				var id_as=$('autoscroll');
				if(id_ts && id_as)
				{
					var ts_top=id_ts.offsetTop-id_ts.scrollTop;
					var as_top=id_as.offsetTop-id_as.scrollTop;
					if(as_top)
						ast=as_top-ts_top-4;
//alert('id_as='+id_as+':\n\nfxwork: oTop='+oID.fxwork.offsetTop+', sTop='+oID.fxwork.scrollTop+' -- topscroll: oTop='+ts_top+' -- autoscroll: oTop='+as_top+'\n\nast='+ast+', asl='+asl);
					oID.fxwork.scrollTop=ast;
					oID.fxwork.scrollLeft=asl;
				}

				if(!otr && !oFXP.tr)
					oID.fxlog.style.display='none';
				else if(oSet.client > 0)
				{
					if(otr && (oFXP.tr <= 0))	// Logout/Login
					{
						oID.fxlog.style.display='none';

						oID.fxsrc.style.display='none';
						oID.fxinf.style.display='none';
						oID.fxset.style.display='none';
						oID.fxflt.style.display='none';
						oID.fxfav.style.display='none';
						oID.fxbak.style.display='none';
						oID.fxhom.style.display='none';

						oID.fxtaction.style.display='none';

						gSet.wfsl=0;
						var nwl=-(parseInt(oID.iworkflow.style.width)+20);
						oID.iworkflow.style.display='none';
						oID.iworkflow.style.left=nwl+'px';

						gSet.prjsl=0;
						var npl=-(parseInt(oID.iprojects.style.width)+20);
						oID.iprojects.style.display='none';
						oID.iprojects.style.left=npl+'px';

						oID.fxtaskp.style.display='none';
						oID.fxtaskl.style.display='none';

						fxf_fn_drawMenu('--');
						oID.fxtasku.style.display='none';
						oID.fxtaskf.style.display='none';

						oFXP.ltr=0;
						fxf_fn_keepUserAlive.delay(oFXP.ldelay, '');
						fxf_fn_resize();
						cresize=false;
					}
					else if(!otr && (oFXP.tr > 0))	// Login
					{
						oID.fxlog.style.display='';

						oID.fxsrc.style.display='';
						oID.fxinf.style.display='';
						oID.fxset.style.display='';
						oID.fxflt.style.display='';
						oID.fxfav.style.display='';

						if(oSet.ccenter > 0)
							oID.fxhom.style.display='';

						oID.fxsrcinput.value='';

						fxf_fn_searchPopup(false);

						if(abc < 2)
							oID.fxtaction.style.display='none';
						else
							oID.fxtaction.style.display='';

						oID.fxtaskl.style.display='';

						fxf_fn_drawMenu('');

						oID.iworkflowi.innerHTML='<table width=100% height=100%><tr><td width=100% height=100% align=center><font class=white>loading&hellip;</font></td></tr></table>';
						oID.iprojectsm.innerHTML='<table width=100% height=100%><tr><td width=100% height=100% align=center><font class=black>loading&hellip;</font></td></tr></table>';

						fxf_fn_keepUserAlive.delay(oFXP.ldelay, '');
						fxf_fn_resize();
						cresize=false;
					}
				}
				if(oFXP.tr && (oFXP.tr != otr) && (otr != oFXP.ltr))
				{
					if(otr > 0)
						oID.fxbak.style.display='';
					oFXP.ltr=otr;
				}
//alert('Length: ajaxTransport='+data[0].length+', innerHTML='+oID.fxwork.innerHTML.length);
			}
			else
			{
				alert('Session changed, deleted or lost? | Error DL'+data.length+'? Reloading...\n\n'+data);
//				document.location.href='index.php';
			}
			if(oID.fxframe.style.display == 'none')
			{
				var fxlogo=$('fxlogo');
				if(fxlogo)
				{
					new Effect.Fade(fxlogo, {from:1.0, to:0.0, duration: 1.0});
					fxf_fn_slideToolbar.delay(1.0, true);
					fxf_fn_workAppear.delay(1.50, true);
					fxf_fn_resize.delay(2.50);
					cresize=false;
				}
				else
				{
					fxf_fn_slideToolbar(false);
					fxf_fn_workAppear(false);
				}
			}
			else
				fxf_fn_workScroll();

			fxf_fn_setPageStatus('green');
			if(cresize)
				fxf_fn_resizeCheck();

			// Adjust document title
			if(oSet.client > 0)
			{
				if(oFXP.tr)
				{
					var ntitle=oFXP.tr;
					if(oID.fxpath && oID.fxpath.innerHTML && oID.fxpath.innerHTML.length)
						ntitle=ntitle+' | '+fxf_fn_stripTags(oID.fxpath.innerHTML).replace(/\&gt\;/g, '>');
				}
				else	// Logout/Login
					var ntitle='Logout/Login';
			}
			else
				var ntitle='Login';
			var otitle=document.title;
			var mpos=otitle.indexOf(' | ');
			if(mpos > 0)
				otitle=otitle.substr(0,mpos);
			ntitle=otitle+' | '+ntitle;
			if((oSet.client > 0) && oID.fxtaskp && oID.fxtaskp.innerHTML.length)
				ntitle += '  ['+oID.fxtaskp.innerHTML+']';
//alert('mpos='+mpos+', otitle='+otitle+', ntitle='+ntitle);
			document.title=ntitle;

			fxf_fn_toggleScreenshotMode(scrshtmode);
		},
		onFailure: function()
		{
//alert('FAILURE');
			fxf_fn_waitScreen('');
		}
	});
}

function fxf_fn_getSelectedValue(element)
{
	var so={value:0, text:''};
	if(element.attributes && element.attributes['svalue'])
	{
		so.value=element.attributes['svalue'].value;
		so.text=element.innerHTML;
		var dio=so.text.indexOf('<div');
		if(dio >= 0)
			so.text=so.text.substr(0,dio);
		else
		{
			var dio=so.text.indexOf('<DIV');
			if(dio >= 0)
				so.text=so.text.substr(0,dio);
		}
	}
//alert('getSelectedValue: so {value:'+so.value+', text:'+so.text+'}');

	return so;
}

function fxf_fn_getSelectedIndex(element)
{
	if(element.attributes && element.attributes['svalue'])
	{
		var val=element.attributes['svalue'].value;
		var field=element.id;
		var ssm=$('sel_'+field);
		if(!ssm)
		{
			var ms=field.indexOf('[');
			if(ms > 0)
				ssm=$('sel_'+field.substr(0,ms));
		}
		if(ssm)
		{
			var lines = ssm.innerHTML.split(divstr[1]);
			if(lines.length)
			{
				for(var i=0; i<lines.length; i++)
				{
					var data=lines[i].split(divstr[0]);
					if(val == data[0])
						return i;
				}
			}
		}
	}

	return -1;
}

function fxf_fn_selectedValue(field,svalue,one)
{
	var ssm=$('sel_'+field);
	if(!ssm)
	{
		var ms=field.indexOf('[');
		if(ms > 0)
			ssm=$('sel_'+field.substr(0,ms));
	}
	if(ssm)
	{
		var lines = ssm.innerHTML.split(divstr[1]);
		if(lines.length)
		{
			for(var i=0; i<lines.length; i++)
			{
				var data=lines[i].split(divstr[0]);
				if(data.length < 2)
					return '?';
				else if(svalue == data[0])
				{
					if(one)
						return data[1];
					return data;
				}
			}
		}
	}

	return '?';
}

function fxf_fn_changeSelectedValue(element,svalue,save)
{
	if(element.attributes && element.attributes['svalue'])
	{
		var ssm=$('sel_'+element.uid);
		if(!ssm)
		{
			var ms=element.uid.indexOf('[');
			if(ms > 0)
				ssm=$('sel_'+element.uid.substr(0,ms));
		}
		if(ssm)
		{
			var lines = ssm.innerHTML.split(divstr[1]);
			if(lines.length)
			{
				for(var i=0; i<lines.length; i++)
				{
					var data=lines[i].split(divstr[0]);
					if(svalue == data[0])
					{
						fxf_fn_drawChangedSelectedValue(element,svalue,data[1]+'|'+svalue,save);
						break;
					}
				}
			}
		}
		else
		{
			var url=fxf_fn_worker('selectvalue','id='+element.uid+'&val='+svalue);
//alert('changeSelectedValue: url='+url);
			new Ajax.Request(url,
			{
				method:'get', asynchronous:asajax,
				onSuccess: function(transport)
				{
//fxf_fn_writeDebug('log', transport.responseText);
					fxf_fn_drawChangedSelectedValue(element,svalue,transport.responseText,save);
					if(element.id == 'Fehltagsart')
					{
//alert('svalue='+svalue+'\ntransport.responseText='+transport.responseText);
						fxf_fn_waitScreen('#');
						fxf_fn_adjustAbsent2.delay(oFXP.mdelay);
					}
				}
			});
		}
	}
}

function fxf_fn_setNewSelects(sne,ret,keepdv)
{
	var dt=fxf_fn_getSelectedValue(sne).text;
	var sf=$('sel_'+sne.id);
	if(sf)
		sf.innerHTML=ret;
	var ra=ret.split(divstr[1]);
	if(ra.length)
	{
		var di=0;
		if(keepdv)
		{
			for(var r=0; r<ra.length; r++)
			{
				var rs=ra[r].split(divstr[0]);
				if(rs[1] == dt)
				{
					di=r;
					break;
				}
			}
		}
		var rs=ra[di].split(divstr[0]);
		var cn3=fxf_fn_getSelType(sne);
		var sea=fxf_fn_getSelStyle(rs[1]);
		sne.innerHTML=sea.tx+'<div class="'+cn3+'slc" style="cursor:pointer;"><span class="'+cn3+'sli"></span></div>';
		sne.attributes['svalue'].value=rs[0];
		sne.fxv=rs[0];
		sne.style.background=sea.bg;
	}
//alert('length='+ra.length+'\n\n'+ret);
}

function fxf_fn_getElement(ename,eindex,etype)
{
	var e=null;
	if(!eindex)
	{
		var n=document.getElementsByName(ename);
		if(!n || (n.length<2) || !etype)
		{
			e=$(ename);
			if(!e)
				e=document.getElementById(ename);
			if(!e)
				e=document.getElementsByName(ename)[0];
		}
		else
		{
			for(var t=0; t<n.length; t++)
			{
				e=n[t];
				if(etype == 'debug')
					alert(e.name+': type='+e.type);
				if(e.type.substr(0, etype.length) == etype)
					break;
			}
		}
	}
	else
	{
		eindex=parseInt(eindex);
		e=document.getElementsByName(ename)[eindex];
	}

	if(e)
		e=fxf_fn_getField(Element.extend(e),true);

	return e;
}

function fxf_fn_getField(element,getval)
{
	if(!element)
		return false;

	var uid='';
	var fxf='';
	var eup=null;
	var sav=true;
	if(element.id && (element.id.substr(0,3) != 'nup') && !element.onclick)
	{
		eup=element.up();
		if(eup)
		{
			var eupcn=fxf_fn_getRealClassName(eup, true);
			if(eupcn.length && ((eupcn.substr(0,3) == 'fxf') || (eupcn.substr(0,3) == 'fmf')))
			{
				element=eup;
				eup=element.up();
			}
		}
	}

	var ecn=fxf_fn_getRealClassName(element, true);
	if(ecn.length && ((ecn.substr(0,3) == 'fxf') || (ecn.substr(0,3) == 'fmf')))
	{
fxf_fn_writeDebug('log+', 'element.className='+ecn);
		fxf='fx';
		if(ecn.substr(0,3) == 'fmf')
			sav=false;
		if((ecn == 'fxfslc') || (ecn == 'fmfslc'))
			element=element.up();
		else if((ecn == 'fxfsli') || (ecn == 'fmfsli'))
			element=element.up().up();
	}

	if(element.name)
		uid=element.name;
	else if(element.id)
		uid=element.id;
	else if(eup)
	{
		var eupcn=fxf_fn_getRealClassName(eup, false);
		if(eupcn.length && (eupcn.substr(0,6) == 'sentry'))
		{
			element=eup;
			uid=element.id;
			ecn=fxf_fn_getRealClassName(element, true);
		}
	}

	element.uid=uid;
	element.fxf=fxf;
	element.sav=sav;
	element.fxv=null;
	if((fxf == 'fx') && element.uid.length)
	{
		var g=ecn.substr(5,1);
		if(g != 'g')
		{
			element.fxf=ecn.substr(3,2);

			if(getval)
			{
				var v=null;
				if((element.fxf == 'tx') || (element.fxf == 'ta') || (element.fxf == 'tc') || (element.fxf == 'pw'))	// || (element.fxf == 'sl'))
					v=element.value;
				else if((element.fxf == 'sl') || (element.fxf == 'sm'))
				{
					if(element.attributes['svalue'])
						v=element.attributes['svalue'].value;
				}
				else if((element.fxf == 'cb') || (element.fxf == 'ra'))
				{
					if(element.checked)
						v=element.value;
					else
						v='';
				}
				element.fxv=v;
			}
		}
	}

	return element;
}

function fxf_fn_writeDebug(target, txt)
{
	if(debug && oDebug && oDebug.fxdebugl)
	{
		if(target == 'log+')
			oDebug.fxdebugl.innerHTML=txt+'<hr style="border:0;border-top:1px solid #aaaabb;">'+oDebug.fxdebugl.innerHTML;
		else if(target == 'log')
			oDebug.fxdebugl.innerHTML=txt;
		else
			oDebug.fxdebugb.innerHTML=txt;
	}
}

function fxf_fn_getRealClassName(element, fxfield)
{
	var ecn='';
	if(element && element.className)
	{
		ecn=element.className;
		if(ecn.substr(0,5) == 'focus')
			ecn=ecn.substr(5);
		if(fxfield && ecn.length)
		{
			var eca=ecn.split(' ');

			ecn='';
			for(var e=0; e<eca.length; e++)
			{
				if((eca[e].substr(0,3) == 'fxf') || (eca[e].substr(0,3) == 'fmf'))
				{
					ecn=eca[e];
					break;
				}
			}
		}
	}

	return ecn;
}

function fxf_fn_sAction(force)
{
	// Tooltip
	if(oID.tooltip && oID.tooltip.style && (oID.tooltip.style.display != 'none'))
	{
		if(mcheck || mmode)
			oID.tooltip.style.display='none';
		else
		{
			var tw=oID.tooltip.clientWidth;
			var th=oID.tooltip.clientHeight;
			var pl=10;
			var tl=Math.max(1, mcx-pl);
			pl=mcx-tl;
			if(tl+tw > dim.sd.pwidth-20)
			{
				tl=dim.sd.pwidth-tw-20;
				pl=mcx-tl;
			}
			var ps=0;
			if(oID.ttpointer.style.display == 'none')
				ps=20;

			oID.tooltip.style.left=tl+'px';
			oID.ttpointer.style.left=Math.max(8, Math.min(pl, tw-16))+'px';
			oID.ttpointer.style.borderTop='';
			oID.ttpointer.style.borderBottom='';
			oID.ttpointer.style.borderRadius='0';
			oID.ttpointer.style.boxShadow='';
			if(mcy-th < 28)
			{
				oID.tooltip.style.top=(mcy+36-ps)+'px';
				oID.ttpointer.style.top='-13px';
				oID.ttpointer.style.bottom='';
				oID.ttpointer.style.borderBottom='1px solid #ffffff';
				oID.ttpointer.style.borderTopRightRadius='100%';
				oID.ttpointer.style.boxShadow='';
			}
			else
			{
				oID.tooltip.style.top=(mcy-th-24+ps)+'px';
				oID.ttpointer.style.top='';
				oID.ttpointer.style.bottom='-13px';
				oID.ttpointer.style.borderTop='1px solid #ffffff';
				oID.ttpointer.style.borderBottomRightRadius='100%';
				oID.ttpointer.style.boxShadow='5px 5px 2px rgba(0,0,0, 0.09)';
			}
		}
	}

	// Line marker
	if((oFXP.tr == 189) && oID.sdg_container && oID.gmain && (oID.iainfo.style.display == 'none'))
	{
		var sdg_offsets=fxf_fn_getBCR(oID.sdg_container);
		var goffsets=fxf_fn_getBCR(oID.gmain);

		var sl=sdg_offsets.left+1;
		var sr=sdg_offsets.right-17;
		var st=goffsets.top;
		var sb=sdg_offsets.bottom-17;

		var gl=goffsets.left;
		var gr=sr;
		var gt=st;
		var gb=sb;

		var tl=oID.smain.scrollTop+mcy-st;
		var cl=Math.floor(tl/dh);
		var rl=fxf_fn_getKeyFromLine(cl);

		// Action menu
		if(adisp && !mmode && !emode)
		{
			if(slarge)
			{
				var sr=sl+parseInt(oID.structure.style.width)-3;
				if(!pArray.length && (mcx > sl) && (mcx < sr) && (mcy > st) && (mcy < sb))
					rsl=-1;
				else if((rl >= 0) && (rl < pArray.length) && (mcx > sl) && (mcx < sr) && (mcy > st) && (mcy < sb) && (pArray[rl].open == 'fo'))
				{
					if(force || (rl != rsl))
						rsl=rl;
				}
				else
					rsl=-1;
if(debug) oDebug.fxdebugm.innerHTML='mcx:'+mcx+', mcy:'+mcy+' -- cl='+cl+', rl='+rl;
			}
			if(glarge)
			{
				var cl=Math.floor((oID.gmain.scrollTop+mcy-gt-1)/dh);
				var rl=fxf_fn_getKeyFromLine(cl);
//fxf_fn_taskMsg('rl='+rl+', pArray.length='+pArray.length+' -- mcx='+mcx+', mcy='+mcy+' -- gl='+gl+', gr='+gr+', gt='+gt+', gb='+gb);

				if((rl > 0) && (rl < pArray.length) && (pArray[rl].type != 'M') && (mcx > gl) && (mcx < gr) && (mcy > gt) && (mcy < gb))
				{
					if(pArray[rl].rights < 4)
					{
						oID.gdaction.style.display='none';
						rgl=-1;
					}
					else
					{
						var pl=pArray[rl].left*zdays[oFXP.zoom];
						var pw=pArray[rl].width*zdays[oFXP.zoom];
						var mx=pl+gl-oID.gmain.scrollLeft;
						var axl=mx-40;
						var axr=mx+Math.floor(pw/8);
						var bxl=mx+pw-Math.floor(pw/8);
						var bxr=mx+pw+40;

						if((mcx > axl) && (mcx < axr))
						{
							oID.gdaction.id='gdpla'+pArray[rl].uid;
							oID.gdaction.style.left=(axl-gl+oID.gmain.scrollLeft)+'px';
							oID.gdaction.style.top=(cl*dh)+'px';
							oID.gdaction.style.display='';
							rgl=rl;
						}
						else if((mcx > bxl) && (mcx < bxr))
						{
							oID.gdaction.id='gdplb'+pArray[rl].uid;
							oID.gdaction.style.left=(bxr-24-gl+oID.gmain.scrollLeft)+'px';
							oID.gdaction.style.top=(cl*dh)+'px';
							oID.gdaction.style.display='';
							rgl=rl;
						}
						else
						{
							oID.gdaction.style.display='none';
							rgl=-1;
						}
					}
				}
				else
				{
					oID.gdaction.style.display='none';
					rgl=-1;
				}
if(debug) oDebug.fxdebugm.innerHTML='mcx:'+mcx+', mcy:'+mcy+' -- cl='+cl+', rl='+rl+' (force='+force+')';
			}
		}
		else
		{
			if(rsl >= 0)
			{
				oID.saction.style.display='none';
				rsl=-1;
			}
			if(rgl >= 0)
			{
				oID.gdaction.style.display='none';
				rgl=-1;
			}
		}

		// Action Fader
		if(slarge && (oID.saction.style.display != 'none'))
		{
			var goff=fxf_fn_getBCR(oID.saction);
			var fvl=1.0;
			if(mcx < goff.left-50)
				fvl=1.0-Math.min(1.0, (goff.left-mcx)/100);
			var fvt=1.0;
			if(mcy < goff.top-50)
				fvt=1.0-Math.min(1.0, (goff.top-mcy)/100);
			var fvr=1.0;
			if(mcx > goff.right+50)
				fvr=1.0-Math.min(1.0, (mcx-goff.right)/100);
			var fvb=1.0;
			if(mcy > goff.bottom+50)
				fvb=1.0-Math.min(1.0, (mcy-goff.bottom)/100);
			var fv=Math.min(fvl,Math.min(fvt,Math.min(fvr,fvb)));
			oID.saction.setOpacity(fv);
		}
		if(glarge && moi.length && (moii >= 0) && (oID.gaction.style.display != 'none'))
		{
			var goff=fxf_fn_getBCR(oID.gaction);
			var coff=fxf_fn_getBCR(pArray[moii].celement);
			goff.left=Math.min(goff.left,coff.left);
			goff.top=Math.min(goff.top,coff.top);
			goff.right=Math.max(goff.right,coff.right);
			goff.bottom=Math.max(goff.bottom,coff.bottom);
			var fvl=1.0;
			if(mcx < goff.left-50)
				fvl=1.0-Math.min(1.0, (goff.left-mcx)/100);
			var fvt=1.0;
			if(mcy < goff.top-50)
				fvt=1.0-Math.min(1.0, (goff.top-mcy)/100);
			var fvr=1.0;
			if(mcx > goff.right+50)
				fvr=1.0-Math.min(1.0, (mcx-goff.right)/100);
			var fvb=1.0;
			if(mcy > goff.bottom+50)
				fvb=1.0-Math.min(1.0, (mcy-goff.bottom)/100);
			var fv=Math.min(fvl,Math.min(fvt,Math.min(fvr,fvb)));
			oID.gaction.setOpacity(fv);
		}
	}
}

function fxf_fn_tooltip(tn, tx)
{
	var tt='';

	if(gSet.help && !Prototype.BrowserFeatures['Mobile'].length && (zObject === null))
	{
		if((tn < 0) && tx.length)
			tt=tx;
		else if(!mcheck && !mmode)
		{
			switch(tn)
			{
				case 5:
					if(oFXP.lang == 1)
						tt='Linke Maustaste gedrückt halten zum Verlängern/Verkürzen der Vorgangsdauer';
					else
						tt='Hold left mouse button to enlarge/shrink process duration';
				break;

				case 6:
					if(oFXP.lang == 1)
						tt='Linke Maustaste gedrückt halten zum Erstellen einer Anfangs-Abhängigkeit,<br />anschließend zum gewünschten Zielvorgang ziehen';
					else
						tt='Hold left mouse button to create a start dependency<br />and move it to desired target process';
				break;

				case 7:
					if(oFXP.lang == 1)
						tt='Linke Maustaste gedrückt halten zum Erstellen einer Ende-Abhängigkeit,<br />anschließend zum gewünschten Zielvorgang ziehen';
					else
						tt='Hold left mouse button to create an end dependency<br />and move it to desired target process';
				break;

				case 8:
					if(oFXP.lang == 1)
						tt='Linke Maustaste gedrückt halten zum Kopieren des Vorgangs,<br />anschließend zur gewünschten Startposition ziehen';
					else
						tt='Hold left mouse button to copy this process<br />and move it to desired start position';
				break;

				case 9:
					if(oFXP.lang == 1)
						tt='Linke Maustaste gedrückt halten zum freien Verschieben des Vorgangs,<br />anschließend zur gewünschten Position ziehen';
					else
						tt='Hold left mouse button to freely move this process<br />and move it to desired position';
				break;

				case 10:
					if(oFXP.lang == 1)
						tt='Vorgang löschen';
					else
						tt='Delete process';
				break;

				case 11:
					if(oFXP.lang == 1)
						tt='Linke Maustaste gedrückt halten zum zeitlichen Verschieben des Vorgangs<br />Doppelklick zum Umbenennen des Vorgangs';
					else
						tt='Hold left mouse button to reposition process<br />Double click to rename process';
				break;

				case 12:
					if(oFXP.lang == 1)
						tt='Doppelklick zum Umbenennen des Vorgangs';
					else
						tt='Double click to rename process';
				break;

				case 13:
					if(oFXP.lang == 1)
						tt='Linke Maustaste gedrückt halten zum vertikalen Verschieben des Vorgangs,<br />anschließend zur gewünschten Position ziehen';
					else
						tt='Hold left mouse button to move this process vertically<br />and move it to desired position';
				break;

				case 14:
					if(oFXP.lang == 1)
						tt='Budgetplanungsfelder neu berechnen anhand des geplanten Aufwands';
					else
						tt='Recalculate budget planning fields on the basis of planned effort';
				break;

				case 15:
					if(oFXP.lang == 1)
						tt='Menü mit Zusatzoptionen öffnen';
					else
						tt='Open menu with additional options';
				break;

				case 21:
					if(oFXP.lang == 1)
						tt='Rechte Maustaste drücken um ein Hauptprojekt anzulegen<br />Taste [F1] drücken um Hilfe- und Support aufzurufen';
					else
						tt='Press right mouse button to create a main project<br />Press [F1] to display help and support';
				break;

				case 24:
					if(oFXP.lang == 1)
						tt='Linke Maustaste gedrückt halten zum zeitlichen Verschieben des Meilensteins<br />Doppelklick zum Umbenennen des Meilensteins';
					else
						tt='Hold left mouse button to reposition milestone<br />Double click to rename milestone';
				break;

				case 25:
					if(oFXP.lang == 1)
						tt='Doppelklick zum Umbenennen des Meilensteins';
					else
						tt='Double click to rename milestone';
				break;

				case 26:
					var oh=parseInt(oID.overview.style.height);
					if(oFXP.lang == 1)
					{
						tt='Navigationsübersicht ';
						if(oh >= 240)
							tt += 'zuklappen';
						else
							tt += 'aufklappen';
					}
					else
					{
						if(oh >= 240)
							tt='Hide';
						else
							tt='Show';
						tt += ' navigation overview';
					}
				break;

				case 27:
					if(oFXP.lang == 1)
						tt='Linke Maustaste gedrückt halten zum Positionieren des Anzeigebereichs';
					else
						tt='Hold left mouse button to reposition display area';
				break;
			}
		}
	}

	if(oID.tooltip && oID.tooltip.style)
	{
		if(tt.length)
		{
			if(tt.substr(0,6) == '<hide>')
			{
				oID.ttbackground.style.display='none';
				oID.ttpointer.style.display='none';
				tt=tt.substr(6);
			}
			else
			{
				oID.ttbackground.style.display='';
				oID.ttpointer.style.display='';
			}

			oID.ttentry.innerHTML=tt;
			oID.tooltip.style.display='';
		}
		else
			oID.tooltip.style.display='none';
	}
	fxf_fn_sAction(true);
}

function fxf_fn_selPopup(sm)
{
	oID.selpop.style.display='none';
	oID.selpopsearch.style.display='none';
	oID.selpoptoggle.style.display='none';
	oID.selpopptype.style.display='none';
	if(sm)
	{
		if(sm.uid == 'fxfltsel')
			fxf_fn_clickFilter(sm);

		var ssm=$('sel_'+sm.uid);
		if(!ssm)
		{
			var ms=sm.uid.indexOf('[');
			if(ms > 0)
				ssm=$('sel_'+sm.uid.substr(0,ms));
		}
		if(ssm)
		{
			var html=fxf_fn_getSelPopup(sm,ssm,sm.attributes['svalue'].value);
			fxf_fn_drawSelPopup(sm,html);
		}
		else
		{
			var url=fxf_fn_worker('select','id='+sm.uid+'&sel='+sm.attributes['svalue'].value);
			new Ajax.Request(url,
			{
				method:'get', asynchronous:true,
				onSuccess: function(transport)
				{
//fxf_fn_writeDebug('log', transport.responseText);
					fxf_fn_drawSelPopup(sm,transport.responseText);
				}
			});
		}
	}
	else
	{
		oVar.selsearch=false;
		oVar.lastSelUID='';
		oVar.lastSelElement=null;
		oVar.lastMSelUID='';
		oVar.lastMSelElement=null;
	}
}

function fxf_fn_dispMenu(event,dm)
{
	if(dm <= 0)
	{
		if((dm < 0) && (oFXP.tr == 189))
			fxf_fn_editStop(event,false);

		if(oID.cmenu && oID.cmenu.style)
			oID.cmenu.style.display='none';
		cmenu=0;

		fxf_fn_tooltip(0);
	}
	fxf_fn_sAction(true);
}

function fxf_fn_worker(fn,param)
{
	var url=fxf_fn_gProgram('worker', 'fn='+fn+fxf_fn_gParam());
	var dbgl=parseInt(fxf_fn_getSelectedValue($('dbgmode')).value);
	if(dbgl != gSet.dbgl)
	{
		gSet.dbgl=dbgl;
		url += '&sdbgl='+gSet.dbgl;
	}
	if(param && param.length)
		url += '&'+param;
//alert('url='+url);

	return url;
}

function fxf_fn_gProgram(url,param)
{
	url='loader.php?url='+url+'.inc';
	if(param && param.length)
		url += '&'+param;
//alert('url='+url);

	return url;
}

function fxf_fn_gParam()
{
	var locstoid=localStorage.getItem('locstoid');
	var sesstoid=sessionStorage.getItem('sesstoid');
	var url='&locstoid='+locstoid+'&sesstoid='+sesstoid+'&locseskey='+oFXP.locseskey+'&lts='+oFXP.lts+'&lcnt='+oFXP.lcnt;
	oFXP.lcnt++;
//alert('url='+url);

	return url;
}

function fxf_fn_unfocus(sel,msel,cel)
{
	if(sel)
	{
		if(oVar.lastSelUID == 'fxfltsel')
			fxf_fn_clickFilter(false);

		var els=$$('.focusfxfsl,.focusfxfslm,.focusfmfsl');
		if(els && els.length)
		{
			for(var e=0; e<els.length; e++)
				els[e].className=els[e].className.substr(5);
		}
		oVar.lastSelUID='';
		oVar.lastSelElement=null;
	}
	if(msel)
	{
		var els=$$('.focusfxfsm,.focusfxfsmm,.focusfmfsm');
		if(els && els.length)
		{
			for(var e=0; e<els.length; e++)
				els[e].className=els[e].className.substr(5);
		}
		oVar.lastMSelUID='';
		oVar.lastMSelElement=null;
	}
	fxf_fn_selPopup(false);
	if(cel)
		cel.focus();
}

function fxf_fn_setMessageStatus(ststr)
{
	var stype=0;
	if(oFXP.ptr)
		var mp='pt';
	else
		var mp='i';

	// 1=Green
	var tlg=parseInt(ststr.substr(2,1));
	var itg=$(mp+'messageatlg');
	if(tlg)
	{
		stype=1;
		itg.style.background='url(GFX/tl_green.png)';
	}
	else
		itg.style.background='url(GFX/tl_neutral.png)';

	// 2=Yellow
	var tly=parseInt(ststr.substr(1,1));
	var ity=$(mp+'messageatly');
	if(tly)
	{
		stype=2;
		ity.style.background='url(GFX/tl_yellow.png)';
	}
	else
		ity.style.background='url(GFX/tl_neutral.png)';

	// 3=Red
	var tlr=parseInt(ststr.substr(0,1));
	var itr=$(mp+'messageatlr');
	if(tlr)
	{
		stype=3;
		itr.style.background='url(GFX/tl_red.png)';
	}
	else
		itr.style.background='url(GFX/tl_neutral.png)';

	// Set border color according to highest status
	var b0=$(mp+'messageg');
	var b1=$(mp+'messageb');
	var b2=$(mp+'messagea');
	if(b0 && b1 && b2)
	{
		var mbca=['ffffff', '1fa97c', 'c1c100', 'e31f35'];
		b0.style.background='#'+mbca[stype];
		b1.style.borderLeftColor='#'+mbca[stype];
		b1.style.borderTopColor='#'+mbca[stype];
		b1.style.borderRightColor='#'+mbca[stype];
		b1.style.borderBottomColor='#'+mbca[stype];
		b2.style.background='#'+mbca[stype];
	}

	return stype;
}

function fxf_fn_growMessage(dur)
{
	if(oFXP.ptr)
	{
		oID.ptmessage.style.display='';
		oID.ptmessageai.style.backgroundPosition='-36px 0';
		new Effect.Morph(oID.ptmessage, {style:'right:0', duration: dur});
	}
	else
	{
		oID.imessage.style.display='';
		oID.imessageai.style.backgroundPosition='-36px 0';
		new Effect.Morph(oID.imessage, {style:'right:0', duration: dur});
	}
}

function fxf_fn_shrinkMessage(dur,force)
{
	if(force || ((oFXP.ptr) && (oID.ptmessage.style.display != 'none')) || ((!oFXP.ptr) && (oID.imessage.style.display != 'none')))
	{
		if(oFXP.ptr)
		{
			oID.ptmessage.style.display='';
			oID.ptmessageai.style.backgroundPosition='-12px 0';
			var nr=-(parseInt(oID.ptmessage.style.width)-3);
			new Effect.Morph(oID.ptmessage, {style:'right:'+nr+'px', duration: dur});
		}
		else
		{
			oID.imessage.style.display='';
			oID.imessageai.style.backgroundPosition='-12px 0';
			var nr=-(parseInt(oID.imessage.style.width)-3);
			new Effect.Morph(oID.imessage, {style:'right:'+nr+'px', duration: dur});
		}
	}
}

function fxf_fn_workAppear(effect)
{
	if(effect)
	{
		new Effect.Appear(oID.fxframe, {from:0.0, to:1.0, duration: 1.0});
		if(oFXP.tr == 189)
			fxf_fn_jumpPosition.delay(oFXP.sdelay, 'sp');
	}
	else
	{
		oID.fxframe.setOpacity(1.0);
		oID.fxframe.style.display='';
	}
	fxf_fn_workScroll.delay(oFXP.sdelay);
}

function fxf_fn_slideToolbar(effect)
{
	if(effect)
	{
		new Effect.Morph(oID.fxtool, {style:'top:0', duration: 0.2});
		new Effect.Morph(oID.fxmain, {style:'top:98px', duration: 0.2});
	}
	else
	{
		oID.fxtool.style.top='0';
		oID.fxmain.style.top='98px';
	}
	gSet.maintop='98px';
}

function fxf_fn_today(with_time, with_milliseconds)
{
	var s=new Date();
	var d=fxf_fn_addLeadingZeros(s.getFullYear(), 4)+fxf_fn_addLeadingZeros(s.getMonth()+1, 2)+fxf_fn_addLeadingZeros(s.getDate(), 2);
	if(((with_time !== undefined) && with_time) || ((with_milliseconds !== undefined) && with_milliseconds))
		d += fxf_fn_addLeadingZeros(s.getHours(), 2)+fxf_fn_addLeadingZeros(s.getMinutes(), 2)+fxf_fn_addLeadingZeros(s.getSeconds(), 2);
	if((with_milliseconds !== undefined) && with_milliseconds)
		d += fxf_fn_addLeadingZeros(s.getMilliseconds(), 3);
//alert('fxf_fn_today()\n-> d='+d);

	return d;
}

function fxf_fn_diffDays(sd, ed)
{
	var s=new Date(parseInt(fxf_fn_cutLeadingZeros(sd.substr(0,4))), parseInt(fxf_fn_cutLeadingZeros(sd.substr(4,2)))-1, parseInt(fxf_fn_cutLeadingZeros(sd.substr(6,2))), 1, 0, 0);
	var e=new Date(parseInt(fxf_fn_cutLeadingZeros(ed.substr(0,4))), parseInt(fxf_fn_cutLeadingZeros(ed.substr(4,2)))-1, parseInt(fxf_fn_cutLeadingZeros(ed.substr(6,2))), 11, 0, 0);
	var d=Math.floor((e.getTime()-s.getTime())/86400000);
//alert('fxf_fn_diffDays('+sd+', '+ed+')\n\nsy='+sd.substr(0,4)+', sm='+sd.substr(4,2)+', sd='+sd.substr(6,2)+'\ney='+ed.substr(0,4)+', em='+ed.substr(4,2)+', ed='+ed.substr(6,2)+'\n\n->s='+s+' s.getTime='+s.getTime()+'\n->e='+e+' e.getTime='+e.getTime()+'\n\ne-d='+(e.getTime()-s.getTime())+'\n\n-> d='+d);

	return d;
}

function fxf_fn_addDays(sd, ad)
{
	var s=new Date(parseInt(fxf_fn_cutLeadingZeros(sd.substr(0,4))), parseInt(fxf_fn_cutLeadingZeros(sd.substr(4,2)))-1, parseInt(fxf_fn_cutLeadingZeros(sd.substr(6,2))), 1, 0, 0);
	var e=new Date(s.getTime() + ad*86400000);
	var d=fxf_fn_addLeadingZeros(e.getFullYear(), 4)+fxf_fn_addLeadingZeros(e.getMonth()+1, 2)+fxf_fn_addLeadingZeros(e.getDate(), 2);
//alert('fxf_fn_addDays('+sd+', '+ad+')\ne='+e+'\n-> d='+d);

	return d;
}

function fxf_fn_getWeekday(sd)
{
	var s=new Date(parseInt(fxf_fn_cutLeadingZeros(sd.substr(0,4))), parseInt(fxf_fn_cutLeadingZeros(sd.substr(4,2)))-1, parseInt(fxf_fn_cutLeadingZeros(sd.substr(6,2))), 1, 0, 0);
	var d=s.getDay();
//alert('fxf_fn_getWeekday('+sd+')\n\-> '+d);

	return d;
}

function fxf_fn_getWeek(sd)
{
	var s=new Date(parseInt(fxf_fn_cutLeadingZeros(sd.substr(0,4))), parseInt(fxf_fn_cutLeadingZeros(sd.substr(4,2)))-1, parseInt(fxf_fn_cutLeadingZeros(sd.substr(6,2))), 1, 0, 0);
	// Get Thursday
	var th=new Date();
	th.setTime(s.getTime() + (3 - ((s.getDay() + 6) % 7)) * 86400000);
	var cwy=th.getFullYear();
	// Get Thursday
	s=new Date(cwy, 0, 4);
	var th1=new Date();
	th1.setTime(s.getTime() + (3 - ((s.getDay() + 6) % 7)) * 86400000);
	var cw=Math.floor(1.5 + (th.getTime() - th1.getTime()) / 86400000 / 7);

	return cw;
}

function fxf_fn_isLeapYear(y)
{
	return ((y > 1582) && ((!(y%400)) || ((!!(y%100)) && (!(y%4)))));
}

function fxf_fn_monthDays(y, m)
{
	while(m > 12)
	{
		m -= 12;
		y++;
	}
	while(m < 1)
	{
		m += 12;
		y--;
	}

	if(m == 2)
	{
		if(fxf_fn_isLeapYear(y))
			return 29;
		return 28;
	}
	else if((m == 4) || (m == 6) || (m == 9) || (m == 11))
		return 30;
	return 31;
}

function fxf_fn_dateFormat(sd)
{
	sd += '';
	if((sd.length == 8) || (sd.length == 14))
		return fxf_fn_convertTimestamp2Date(sd).date;
	if(sd.length < 2)
		return '';
	return sd;
}

function fxf_fn_string2float(str)
{
	if(!str)
		var str_len=0;
	else
		var str_len=str.length;

	var sep			= false;
	var neg			= false;
	var c			= "";
	var float_left	= "";
	var float_right	= "";
	var tag			= false;
//alert("strlen(str)="+str.length);
	for(i=0; i<str_len; i++)
	{
		c=str.charAt(i);
		if(c == '<')
			tag=true;
		else if(c == '>')
			tag=false;
		else if(!tag)
		{
			if(c == oSet.decimalpoint)
				sep=true;
			else if((c >= "0") && (c <= "9"))
			{
				if(sep)
					float_right=float_right.concat(c);
				else
					float_left=float_left.concat(c);
			}
			else if(c == '-')
				neg=true;
		}
//alert("str.substr("+i+", 1)='"+c+"' -- float_left='"+float_left+"', float_right='"+float_right+"'");
	}
	if(float_left.length == 0)
		float_left="0";
	while(float_right.length < oSet.decimallength)
		float_right=float_right.concat("0");
	float_right=float_right.substr(0, oSet.decimallength);
//alert("float_left='"+float_left+"', float_right='"+float_right+"'");

	if(neg)
		return parseFloat("-"+float_left+"."+float_right);
	return parseFloat(float_left+"."+float_right);
}

function fxf_fn_float2string(flt, dec)
{
	var str=(flt+"").trim();
	var neg=false;
	if((flt < 0.0) || (str.substr(0,1) == '-'))
	{
		neg=true;
		str=str.substr(1);
	}
	var str_int=parseInt(str);
	var str_left=str_int+"";
	var str_right=str.substr(str_left.length+1);

	var dec_length=oSet.decimallength;
	if(dec != undefined)
		dec_length=parseInt(dec);

	var t_sep=".";
	if(oSet.decimalpoint == ".")
		t_sep=",";

	var str_ret="";
	var c="";
	for(si=0; si<str_left.length; si++)
	{
		c=str_left.charAt(si);
		str_ret=str_ret.concat(c);
		if((str_left.length-si-1 > 0) && ((str_left.length-si-1)%3 == 0))
			str_ret=str_ret.concat(t_sep);
	}
	if(dec_length > 0)
	{
		str_ret=str_ret.concat(oSet.decimalpoint);
		for(ri=0; (ri<str_right.length) && (ri<dec_length); ri++)
		{
			c=str_right.charAt(ri);
			str_ret=str_ret.concat(c);
		}
		while(ri < dec_length)
		{
			str_ret=str_ret.concat("0");
			ri++;
		}
	}

//alert("str='"+str+"' ("+typeof(str)+"), neg='"+neg+"' ("+typeof(neg)+"), str_int='"+str_int+"' ("+typeof(str_int)+"), str_left='"+str_left+"' ("+typeof(str_left)+"), str_right='"+str_right+"' ("+typeof(str_right)+"), t_sepr='"+t_sep+"' -- str_ret='"+str_ret+"' ("+typeof(str_ret)+")");
	if(neg)
		return "-"+str_ret;
	return str_ret;
}

function fxf_fn_time2sec(str)
{
	var str_len=str.length;
	var vs='';
	var ts='';
	var tag=false;
	for(i=0; i<str_len; i++)
	{
		c=str.charAt(i);
		if(c == '<')
			tag=true;
		else if(c == '>')
			tag=false;
		else if(!tag)
		{
			if((c >= "0") && (c <= "9"))
				vs=vs.concat(c);
			else if((c == ':') || (c == ' '))
			{
				if(!vs.length)
					vs='00';
				else if(vs.length == 1)
					vs='0'+vs;
				else
					vs=vs.substr(0,2);
				ts=ts.concat(vs);
				vs='';
			}
		}
	}
	if(vs.length)
	{
		if(vs.length == 1)
			vs='0'+vs;
		else
			vs=vs.substr(0,2);
		ts=ts.concat(vs);
	}
	ts=ts.concat('000000');
	ts=ts.substr(0,6);

	var h=parseInt(fxf_fn_cutLeadingZeros(ts.substr(0,2)))*3600;
	var m=parseInt(fxf_fn_cutLeadingZeros(ts.substr(2,2)))*60;
	var s=parseInt(fxf_fn_cutLeadingZeros(ts.substr(4,2)));

	var t=Math.min(86400,h+m+s);

//alert('time2sec: ts='+ts+' > t='+t);
	return t;
}

function fxf_fn_sec2time(sec, alz, ds)
{
	if((alz === undefined) || alz)
		alz=true;
	else
		alz=false;
	if(alz)
		sec=Math.min(86400,sec);

	if((ds === undefined) || !ds)
		ds=false;
	else
		ds=true;

	var ls='';
	if(sec < 0)
	{
		ls='-';
		sec=Math.abs(sec);
	}

	var h=Math.floor(sec/3600);
	var m=Math.floor((sec - h*3600)/60);
	var s=sec - h*3600 - m*60;

	if(!ds && (s > 30))
	{
		s=0;
		m++;
		if(m > 59)
		{
			m=0;
			h++;
		}
	}

	var t=ls;
	if(alz)
		t=t+fxf_fn_addLeadingZeros(h,2);
	else
		t=t+fxf_fn_float2string(h,0);
	t=t+':'+fxf_fn_addLeadingZeros(m,2);
	if(ds)
		t=t+':'+fxf_fn_addLeadingZeros(s,2);

//alert('sec2time: sec='+sec+' (h='+h+', m='+m+', s='+s+') > t='+t);
	return t;
}

function fxf_fn_cutLeadingZeros(val)
{
	if(!val)
		return 0;
	while(val.length && (val.substr(0,1) == '0'))
		val=val.substr(1);
	if(!val.length)
		val='0';

	return parseInt(val);
}

function fxf_fn_addLeadingZeros(val, len)
{
	val=val.toString();
	if(len && (val.length < len))
	{
		while(val.length < len)
			val="0"+val;
	}
	val=val.substring(val.length-len);

	return val;
}

function fxf_fn_decFormat(n)
{
	var r=(Math.round(n * 100) / 100).toString();
	r += (r.indexOf('.') == -1) ? '.00' : '00';
	var p=r.indexOf('.');
	var m=r.indexOf('-.');
	var f=(p == 0 || m == 0) ? '0'+oSet.decimalpoint : oSet.decimalpoint;

	return r.substring(0, p) + f + r.substring(p+1, p+3);
}

function fxf_fn_countWeekdays(sd, ed, wwarray, hdays)
{
	var s=new Date(parseInt(fxf_fn_cutLeadingZeros(sd.substr(0,4))), parseInt(fxf_fn_cutLeadingZeros(sd.substr(4,2)))-1, parseInt(fxf_fn_cutLeadingZeros(sd.substr(6,2))), 1, 0, 0);
	var stm=s.getTime();	// Unix timestamp start date
	var swd=s.getDay();	// Weekday start date (0=Sunday,..,6=Saturday)

	var e=new Date(parseInt(fxf_fn_cutLeadingZeros(ed.substr(0,4))), parseInt(fxf_fn_cutLeadingZeros(ed.substr(4,2)))-1, parseInt(fxf_fn_cutLeadingZeros(ed.substr(6,2))), 11, 0, 0);
	var etm=e.getTime();	// Unix timestamp end date
	var ewd=e.getDay();	// Weekday end date (0=Sunday,..,6=Saturday)

	var cstm=0;
	var cetm=0;
	var wds=0;
	for(var cwd=0; cwd<7; cwd++)
	{
		var wd=parseInt(wwarray[cwd]);
		if(wd > 0)
		{
			if(cwd < swd)
				cstm=stm + (7-swd+cwd) * 86400000;
			else if(cwd > swd)
				cstm=stm + (cwd-swd) * 86400000;
			else
				cstm=stm;

			cetm=etm - Math.abs(7-cwd+ewd)%7 * 86400000;

			wds += 1 + (cetm-cstm) / 604800000;
		}
//alert('cwd='+cwd+' - wd='+wd+'\n\nstm='+stm+', swd='+swd+' - etm='+etm+', ewd='+ewd+'\n\ncstm='+cstm+', cetm='+cetm+'\n\n->wds='+wds);
	}

	wds=Math.round(wds);

	// Calculate holidays
	if(hdays && hArray.length)
	{
		var sdo=fxf_fn_diffDays(sdate, sd);
		var edo=fxf_fn_diffDays(sdate, ed);
		for(var hc=0; hc<hArray.length; hc++)
		{
			if((hArray[hc].type > 0.0) && (hArray[hc].offset >= sdo) && (hArray[hc].offset <= edo) && (workweek[hArray[hc].day]))
				wds -= hArray[hc].type;
		}
	}

	return wds;
}

function fxf_fn_isWeekday(sd, wwarray, hdays)
{
	var s=new Date(parseInt(fxf_fn_cutLeadingZeros(sd.substr(0,4))), parseInt(fxf_fn_cutLeadingZeros(sd.substr(4,2)))-1, parseInt(fxf_fn_cutLeadingZeros(sd.substr(6,2))), 1, 0, 0);
	var stm=s.getTime();	// Unix timestamp start date
	var swd=s.getDay();	// Weekday start date (0=Sunday,..,6=Saturday)
	var etm=stm;
	var ewd=swd;

	var cstm=0;
	var cetm=0;
	var wds=0;
	for(var cwd=0; cwd<7; cwd++)
	{
		var wd=parseInt(wwarray[cwd]);
		if(wd > 0)
		{
			if(cwd < swd)
				cstm=stm + (7-swd+cwd) * 86400000;
			else if(cwd > swd)
				cstm=stm + (cwd-swd) * 86400000;
			else
				cstm=stm;

			cetm=etm - Math.abs(7-cwd+ewd)%7 * 86400000;

			wds += 1 + (cetm-cstm) / 604800000;
		}
//alert('cwd='+cwd+' - wd='+wd+'\n\nstm='+stm+', swd='+swd+' - etm='+etm+', ewd='+ewd+'\n\ncstm='+cstm+', cetm='+cetm+'\n\n->wds='+wds);
	}

	wds=Math.round(wds);

	// Calculate holidays
	if(hdays && hArray.length)
	{
		var sdo=fxf_fn_diffDays(sdate, sd);
		for(var hc=0; hc<hArray.length; hc++)
		{
			if((hArray[hc].type > 0.0) && (hArray[hc].offset == sdo) && (workweek[hArray[hc].day]))
				wds -= hArray[hc].type;
		}
	}

	return wds;
}

function fxf_fn_unitIndex(effort_unit)
{
	var fxp_unit_selected=$(effort_unit).value;
//alert("fxp_unit_selected="+fxp_unit_selected+" ("+typeof(fxp_unit_selected)+")");

	for(ui=0; ui<fxp_unit_select.length; ui++)
	{
		if(fxp_unit_select[ui] == fxp_unit_selected)
			return ui;
	}

	return 0;
}

function fxf_fn_getUnit(lu)
{
	var nu=0;
	for(var u=1; u<oSet.unitselect.length; u++)
	{
		if(oSet.unitselect[u] == lu)
		{
			nu=u;
			break;
		}
	}
	if(!nu)
		return gSet.dunit;
	return nu;
}

function fxf_fn_sec2unit(val,dunit)
{
	dunit=fxf_fn_getUnit(dunit);
	var unit=fxf_fn_float2string(parseInt(val)/oSet.unitcalc[dunit]);
	return unit;
}

function fxf_fn_unit2sec(val,dunit)
{
	dunit=fxf_fn_getUnit(dunit);
	var sec=parseInt(fxf_fn_string2float(val)*oSet.unitcalc[dunit]);
	return sec;
}

function fxf_fn_getDateformat()
{
	if(!oSet.dateformat || !oSet.dateformat.length)
	{
		if(oFXP.lang == 1)
			df='dd.mm.yyyy';
		else
			df='mm/dd/yyyy';
	}
	else
		var df=oSet.dateformat;

	var llang=$('log_lang');
	if(llang)
	{
		var ll=parseInt(llang.innerHTML);
		if(ll != oFXP.lang)
		{
			if(ll == 1)
				df='dd.mm.yyyy';
			else
				df='mm/dd/yyyy';
		}
	}
	return df;
}

function fxf_fn_getTimestamp(year, month, day)
{
	return fxf_fn_addLeadingZeros(year, 4)+fxf_fn_addLeadingZeros(month, 2)+fxf_fn_addLeadingZeros(day, 2);
}

function fxf_fn_convertDate2Timestamp(date)
{
	var adate=new Date();
	var add=adate.getDate();
	var adm=adate.getMonth()+1;
	var ady=adate.getFullYear();
	var adh=adate.getHours();
	var adi=adate.getMinutes();

	var dspos=oSet.dateformat.indexOf('m');
	if(!dspos)
		dspos=oSet.dateformat.indexOf('d')-1;
	else
		dspos -= 1;
	var ds=oSet.dateformat.charAt(dspos);
	var dfa=oSet.dateformat.split(ds);
	var dsa=date.split(ds);
	var ty=fxf_fn_addLeadingZeros(ady, 4), tm=fxf_fn_addLeadingZeros(adm, 2), td=fxf_fn_addLeadingZeros(add, 2);
	for(var di=0; di<3; di++)
	{
		var dt=dfa[di].charAt(0);
		var dv=fxf_fn_cutLeadingZeros(dsa[di]);
		if(dt == 'y')
		{
			if(!dsa[di])
				dv=ady;
			else if(dv < 40)
				dv += 2000;
			else if(dv < 100)
				dv += 1900;
			else if(dv < 1000)
				dv += 1000;
			else
				dv=Math.min(dv, 9999);
			if(isNaN(dv))
				dv=ady;
			ty=fxf_fn_addLeadingZeros(dv, 4);
		}
		else if(dt == 'm')
		{
			if(!dsa[di])
				dv=adm;
			else
				dv=Math.max(Math.min(dv, 12), 1);
			if(isNaN(dv))
				dv=adm;
			tm=fxf_fn_addLeadingZeros(dv, 2);
		}
		else
		{
			if(!dsa[di])
				dv=add;
			else
				dv=Math.max(Math.min(dv, 31), 1);
			if(isNaN(dv))
				dv=add;
			td=fxf_fn_addLeadingZeros(dv, 2);
		}
		dsa[di]=fxf_fn_addLeadingZeros(dv, dfa[di].length);
	}
//alert('[0]:'+dsa[0]+', [1]:'+dsa[1]+', [2]:'+dsa[2]+' - ty:'+ty+', tm:'+tm+', td:'+td);

	return {date: dsa[0]+ds+dsa[1]+ds+dsa[2], timestamp: ty+tm+td};
}

function fxf_fn_convertTimestamp2Date(timestamp, df)
{
	if((typeof df == 'undefined') || (df == ''))
		df=fxf_fn_getDateformat();
	var dspos=df.indexOf('m');
	if(!dspos)
		dspos=df.indexOf('d')-1;
	else
		dspos -= 1;
	var ds=df.charAt(dspos);
	var dfa=df.split(ds);
	var dsa=Array('0', '0', '0');
	var ty=timestamp.substr(0, 4), tm=timestamp.substr(4, 2), td=timestamp.substr(6, 2);
	for(var di=0; di<3; di++)
	{
		var dt=dfa[di].charAt(0);
		if(dt == 'y')
			dv=fxf_fn_cutLeadingZeros(ty);
		else if(dt == 'm')
			dv=fxf_fn_cutLeadingZeros(tm);
		else
			dv=fxf_fn_cutLeadingZeros(td);
		dsa[di]=fxf_fn_addLeadingZeros(dv, dfa[di].length);
	}
//alert('[0]:'+dsa[0]+', [1]:'+dsa[1]+', [2]:'+dsa[2]+' - ty:'+ty+', tm:'+tm+', td:'+td);

	return {date: dsa[0]+ds+dsa[1]+ds+dsa[2], timestamp: ty+tm+td};
}

function fxf_fn_dec2hex(dec)
{
	return hexstr.substr((dec & 0x0000f0)>>4, 1)+hexstr.substr((dec & 0x00000f), 1);
}

function fxf_fn_dateDiff(sdate, edate)
{
	var sy=parseInt(fxf_fn_cutLeadingZeros(sdate.substr(0, 4))), sm=parseInt(fxf_fn_cutLeadingZeros(sdate.substr(4, 2))), sd=parseInt(fxf_fn_cutLeadingZeros(sdate.substr(6, 2)));
	var ey=parseInt(fxf_fn_cutLeadingZeros(edate.substr(0, 4))), em=parseInt(fxf_fn_cutLeadingZeros(edate.substr(4, 2))), ed=parseInt(fxf_fn_cutLeadingZeros(edate.substr(6, 2)));
	var sms=Date.UTC(sy, sm-1, sd, 12, 0, 0);
	var ems=Date.UTC(ey, em-1, ed, 12, 0, 0);

	return parseInt((ems-sms)/86400000);
}

function fxf_fn_escapeText(txt,tosymbol)
{
	if(txt && txt.length)
	{
		if(tosymbol)
		{
			txt=txt.replace(/\(\(lt\)\)/g, '<');
			txt=txt.replace(/\(\(gt\)\)/g, '>');
			txt=txt.replace(/\(\(euro\)\)/g, '&euro;');
			txt=txt.replace(/\(\(plus\)\)/g, '+');
			txt=txt.replace(/\(\(equal\)\)/g, '=');
			txt=txt.replace(/\(\(and\)\)/g, '&');
			txt=txt.replace(/\(\(quot\)\)/g, '"');
			txt=txt.replace(/\(\(apos\)\)/g, '\'');
			txt=txt.replace(/\(\(backslash\)\)/g, '\\');
		}
		else
		{
			txt=txt.replace(/\</g, '((lt))');
			txt=txt.replace(/\>/g, '((gt))');
			txt=txt.replace(/\€/g, '((euro))');
			txt=txt.replace(/\&euro\;/g, '((euro))');
			txt=txt.replace(/\+/g, '((plus))');
			txt=txt.replace(/\=/g, '((equal))');
			txt=txt.replace(/\&/g, '((and))');
			txt=txt.replace(/\"/g, '((quot))');
			txt=txt.replace(/\'/g, '((apos))');
			txt=txt.replace(/\\/g, '((backslash))');
			txt=encodeURIComponent(txt);
		}
	}
	return txt;
}

function fxf_fn_saveLastElement(element)
{
	element=fxf_fn_getField(element,true);
	if(element && element.fxf.length && (element.fxf != 'fx') && oVar.lel && (oVar.lel.id == element.id) && (element.fxv != oVar.fxv))
	{
fxf_fn_writeDebug('log+', '<b style="color:#cc0011;">fxf_eh_blur</b>:<br />id='+element.id+', name='+element.name+', type='+element.type+'<br />-&gt;uid='+element.uid+', fxf='+element.fxf+', fxv='+element.fxv+', oVar.fxv='+oVar.fxv);
		fxf_fn_saveElement(element);
	}
	oVar.lel=null;
}

function fxf_fn_saveElement(element)
{
	oVar.lel=null;
	oVar.fxv=null;

	if(element.fxf == 'tf')
	{
		fEl=null;
		return;
	}

	// Filter
	if(element.id == 'fxfltsel')
	{
		fxf_fn_filterChanged();
		return;
	}

	if(element.attributes['sec'])
		element.attributes['sec'].value=fxf_fn_timespan2sec(element.value, element.id);

	if(element.id == 'pprefix')
	{
		element.value=element.value.trim();
		while(true)
		{
			var dsp=element.value.indexOf('  ');
			if(dsp > 0)
				element.value=element.value.replace(/\ \ /g, ' ');
			else
				break;
		}
		element.value=element.value.replace(/\ /g, '-');
		while(true)
		{
			var dsp=element.value.indexOf('--');
			if(dsp > 0)
				element.value=element.value.replace(/\-\-/g, '-');
			else
				break;
		}
		element.fxv=element.value;
	}

	// Call program or default save function
	var pse=1;
	var trjsa=$$('[id^="trjsa_"]');
//alert('trjsa='+trjsa);
	if(trjsa && trjsa.length)
	{
		var jis='';
		var trjst=$('trjst_'+oFXP.tr);
		if(trjst)
			jis=trjst.innerHTML;

		for(var j=0; j<trjsa.length; j++)
		{
			var jstr=parseInt(trjsa[j].id.substr(6,3));
			if((oFXP.tr == jstr) || (jis.indexOf('|'+jstr+'|') >= 0))
			{
				var sf='fxf_fn_saveElement'+jstr;
				var tf=eval('typeof('+sf+')');
//alert('tf = typeof('+sf+') = '+tf);
				if(tf === 'function')
				{
					eval('pse='+sf+'(element);');
//alert('pse='+pse+' (typeof(pse)='+typeof(pse)+')');
					if(typeof(pse) != 'number')
						pse=0;
					if(pse < 0)
						break;
				}
			}
		}
	}
	if(pse < 0)
		return;
	else if(pse > 0)
		fxf_fn_ajaxSaveElement(element);

	// Column select has changed
	if(element.id.substr(0,7) == 'rcmain_')
	{
		fxf_fn_rcSelect(element,element.id.substr(7));
	}
	// Column select popup has changed
	else if(element.id.substr(0,6) == 'rcsel_')
	{
		fxf_fn_rcSelect(element,element.id.substr(6));
	}
	// Column select checkbox has changed
	else if(element.id.substr(0,5) == 'rccb_')
	{
		var rcf=element.id.split('_');
		var fl=rcf[2];
		for(var i=3; i<rcf.length; i++)
			fl += '_'+rcf[i];
		fxf_fn_rcAction(rcf[1],-1,fl);
	}
	// Month has changed
	else if(element.id.substr(0,10) == 'cal_month_')
	{
		var fname=element.id.substr(10);
		var fdate=$('cal_fdate_'+fname).innerHTML;
		var y=parseInt(fdate.substr(0,4));
		var m=parseInt(element.fxv);
		var d=parseInt(fdate.substr(6,2));
		fxf_fn_changeMonth(fname,y,m,d);
	}
	// Year has changed
	else if(element.id.substr(0,9) == 'cal_year_')
	{
		var fname=element.id.substr(9);
		var fdate=$('cal_fdate_'+fname).innerHTML;
		var y=parseInt(element.fxv);
		var m=parseInt(fdate.substr(4,2));
		var d=parseInt(fdate.substr(6,2));
		fxf_fn_changeYear(fname,y,m,d);
	}
}

function fxf_fn_ajaxSaveElement(element)
{
	if(element.fxf && element.fxf.length && (element.fxf != 'fx') && element.sav)
	{
fxf_fn_writeDebug('info', 'ajaxSaveElement: '+element.uid+' ('+element.fxf+') - {'+element.fxv+'}');
		var ev=element.fxv;
		if((element.fxf == 'pw') || (element.fxf == 'tc')) ev=fxf_fn_ajaxTC(ev);
		var frm='';
		if(element.attributes && (typeof element.attributes['fxform'] != 'undefined'))
			frm=element.attributes['fxform'].value;
		var url=fxf_fn_worker('sfield','keep_post=1&frm='+frm+'&uid='+element.uid+'&fxf='+element.fxf);
		var post='val='+fxf_fn_escapeText(ev,false);
//alert('fxf_fn_ajaxSaveElement:\nurl='+url+'\npost='+post);
		fxf_fn_ajaxSave(url,post);
	}
}

function fxf_fn_ajaxTC(tc)
{
	var o=Math.floor((Math.random()*145)+5);
	var ov='000'+o;
	var rc='*#'+ov.substr(ov.length-3, 3);
	for(var i=0;i<tc.length; i++)
	{
		var cv='000'+(parseInt(tc.charCodeAt(i))+o);
		rc += cv.substr(cv.length-3, 3);
		o += 3;
	}
	return rc+'#*';
}

function fxf_fn_ajaxSave(url,post)
{
	if(oFXP.busy > 0.0)
	{
		fxf_fn_ajaxSave.delay(oFXP.busy, url,post);
		return;
	}

	oFXP.busy += 1.0;
//alert('url='+url+'\npost='+post);
	new Ajax.Request(url,
	{
		method:'post', postBody:post, asynchronous:false,
		onSuccess: function(transport)
		{
//fxf_fn_writeDebug('log', transport.responseText);
			oFXP.busy -= 1.0;
			if(oFXP.addchg)
				oFXP.changed++;
		},
		onFailure: function(failure)
		{
			oFXP.busy -= 1.0;
		}
	});
}

function fxf_fn_fxSubmitButton(element)
{
fxf_fn_writeDebug('info', 'Submit: Click on button ['+element.uid+']');

	if(oFXP.busy > 0.0)
	{
		fxf_fn_fxSubmitButton.delay(oFXP.busy, element);
		return;
	}

	// Click on [Create pdf again]
	if(element.uid == 'pdfs_button')
	{
		// Get all elements that start with "pdfs_"
		var pels = $$('[id^="pdfs_"]');
//alert('pels='+pels);
		fxf_fn_fxLinkClose();
		fxf_fn_waitScreen('reloading');
		if(pels.length)
		{
			var pc='';
			var pp=element.attributes['href'].value.indexOf('&pccnt=');
			if(pp >= 0)
			{
				pc=element.attributes['href'].value.substring(pp);
				element.attributes['href'].value=element.attributes['href'].value.substring(0,pp);
			}
			pp=element.attributes['href'].value.indexOf('&pdfs_');
			if(pp >= 0)
				element.attributes['href'].value=element.attributes['href'].value.substring(0,pp);
//var pt='';
			for(var p=0;p<pels.length;p++)
			{
				var val=pels[p].value;
				if((val == undefined) && pels[p].attributes && pels[p].attributes['svalue'] && pels[p].attributes['svalue'].value)
					val=pels[p].attributes['svalue'].value;
//alert(p+': '+pels[p].id+' (type='+pels[p].type+'), value='+pels[p].value+', val='+val);	//attributes[value]='+pels[p].attributes['value'].value+'\n';
//pt += p+': '+pels[p].id+' (type='+pels[p].type+'), value='+pels[p].value+', val='+val+'\n';
				if(pels[p].type == 'checkbox')
				{
					if(pels[p].checked)
						element.attributes['href'].value += '&'+pels[p].id+'=1';
				}
				else
				{
					element.attributes['href'].value += '&'+pels[p].id+'=';
					if(val.length)
						element.attributes['href'].value += fxf_fn_escapeText(val,false);
				}
			}
			element.attributes['href'].value += pc;
//alert('Elements are:\n'+pt);
		}
//alert('href='+element.attributes['href'].value);
		fxf_fn_fxLink.delay(0.05, element);
	}
	// Click on [Preview]
	else if(element.uid == 'Button_Vorschau')
	{
		var id_ctype=fxf_fn_getField($('ktyp'),true);
		var ctype=parseInt(fxf_fn_getSelectedValue(id_ctype).value);

		var id_ctext=fxf_fn_getField($('Text_varLang'));
		var ctext=id_ctext.value;
//alert('ctype:\n'+ctype+'\n\nctext:\n'+ctext);

		if(oFXP.pwindow)
			oFXP.pwindow.close();

		// Open preview window
		var pform=document.createElement('form');
		var pms=new Date().getTime();
		var pwindowname='Preview'+pms;
		pform.id=pwindowname;
		pform.action=fxf_fn_gProgram('config_preview', 'ctype='+ctype+fxf_fn_gParam());
		pform.method='POST';
		pform.target=pwindowname;
		var phidden=document.createElement('input');
		phidden.type='hidden';
		phidden.name='ctext';
		phidden.value=encodeURIComponent(ctext);
		pform.appendChild(phidden);
		document.body.appendChild(pform);
    	oFXP.pwindow=window.open('', pwindowname, 'left='+(200+screen.availLeft)+',top=100,width='+(dim.sd.pwidth-400)+',height='+(dim.sd.pheight-200)+',toolbar=no,location=no,directories=no,status=no,menubar=no,scrollbars=yes,copyhistory=no,resizable=yes');
		pform.submit();
		document.body.removeChild(pform);
	}
	// Click on [Resource Planning]
	else if((oFXP.tr == 23) && (element.uid == 'Button_Ma_Pro'))
	{
		var pel=fxf_fn_getField($('Projekt_IDopen'),false);
		var pid=parseInt(fxf_fn_getSelectedValue(pel).value);
		fxf_fn_waitScreen('loading');
		fxf_fn_loadTR.delay(oFXP.sdelay, 43,'newtr&aktion=1&gproject='+pid);
	}
	// Click on PP/SP Popup [OK]
	else if(element.uid == 'ppspok')
		fxf_fn_ppspClose(true);
	// Click on Project Download
	else if(element.uid == 'download')
		window.location=element.attributes['get'].value;
	else
	{
		var ei6=element.id.substr(0,6);

		// Click on column select: default
		if(ei6 == 'rcdef_')
		{
			fxf_fn_rcAction(element.id.substr(6),0,false);
		}
		// Click on column select: all
		else if(ei6 == 'rcall_')
		{
			fxf_fn_rcAction(element.id.substr(6),1,false);
		}
		// Click on column select: reverse
		else if(ei6 == 'rcrev_')
		{
			fxf_fn_rcAction(element.id.substr(6),-2,false);
		}
		else
		{
			fxf_fn_fxLinkClose();
			var uid=element.uid;
			if(uid == 'dbtinstall')
				fxf_fn_waitScreen('Installing');
			else if(uid == 'dbtupdate')
				fxf_fn_waitScreen('Updating');
			else if(uid == 'dbtpatch')
				fxf_fn_waitScreen('Patching');
			else if(element.value && ((element.value == 'Speichern') || (element.value == 'Save')))
				fxf_fn_waitScreen('Saving');
			else if((element.id.substr(0,5) == 'purge') || ((oFXP.tr == 283) && (oFXP.action == 2) && (element.id.substr(0,16) == 'Button_Speichern')))
				fxf_fn_waitScreen('Purging');
			else if((oFXP.tr == 250) && (oFXP.action == 2) && (element.id.substr(0,16) == 'Button_Speichern'))
				fxf_fn_waitScreen('Backing Up');
			else if((element.id.substr(0,9) == 'hm_action') || (((oFXP.tr == 250) || (oFXP.tr == 283)) && (oFXP.action == 3) && (element.id.substr(0,16) == 'Button_Speichern')))
				fxf_fn_waitScreen('Restoring');
			else if(element.id.substr(0,15) == 'Button_Loeschen')
				fxf_fn_waitScreen('Deleting');
			else
				fxf_fn_waitScreen('Loading');
			if(uid == 'mtx_save')
			{
				if(fxf_fn_hrPPSPCheck())
					return;
				pcc=true;
				fxf_fn_waitScreen('saving');
				fxf_fn_saveProject('*',false);
			}
			if(element.attributes && (typeof element.attributes['fxform'] != 'undefined'))
				uid=uid+'&frm='+element.attributes['fxform'].value;
			if(element.attributes && (typeof element.attributes['get'] != 'undefined'))
				uid=uid+'&'+element.attributes['get'].value;
			var dtr=oFXP.tr;
			if(element.attributes && (typeof element.attributes['tr'] != 'undefined'))
				dtr=element.attributes['tr'].value;
			if((oFXP.tr == 83) && (element.uid == 'Button_Speichern') && (tSet.enc > 0) && (tSet.sb.length || tSet.sw.length))
			{
				fxf_fn_waitScreen('');
				var dcs='';
				var els=$$('[id^="trw_s_"]');
				if(els.length)
				{
//alert('els='+els);
					for(var e=0; e<els.length; e++)
					{
						if((els[e].id.substr(els[e].id.length-3) == '_du') && (els[e].style.color.substr(4,3) == '225'))
						{
							if(dcs.length)
								dcs += '&nbsp;&nbsp;+&nbsp;&nbsp;';
							dcs += '<b class="red">'+fxf_fn_convertTimestamp2Date(els[e].id.substr(6,8)).date+'</b>';
						}
					}
				}
//alert('dcs='+dcs);
				var msg='';
				if(dcs.length)
					msg += dcs+'<br /><br /><hr class="fxhr" style="border-color:#cccccc;"><br />';
				if(tSet.sb.length)
					fxf_fn_question('', msg+tSet.sb, ['OK'], [''], 100);
				else
					fxf_fn_question('', msg+tSet.sw, [fxf_fn_getText(10), fxf_fn_getText(11)], ['fxf_fn_waitScreen(\'saving\');fxf_fn_loadTR('+dtr+',\''+uid+'\')', ''], 100);
				return;
			}
			fxf_fn_loadTR.delay(oFXP.sdelay, dtr,uid);
		}
	}
}

function fxf_fn_fxResetButton(element)
{
//alert('Reset: Click on button ['+element.uid+']');
}

function fxf_fn_fxSubmitImage(element)
{
fxf_fn_writeDebug('info', 'Submit: Click on image ['+element.uid+']');
	fxf_fn_waitScreen('loading');
	var uid=element.uid+'&uidc=is';
	if(element.attributes && (typeof element.attributes['fxform'] != 'undefined'))
		uid=uid+'&frm='+element.attributes['fxform'].value;
	if(element.attributes && (typeof element.attributes['get'] != 'undefined'))
		uid=uid+'&'+element.attributes['get'].value;
	var dtr=oFXP.tr;
	if(element.attributes && (typeof element.attributes['tr'] != 'undefined'))
		dtr=element.attributes['tr'].value;
	fxf_fn_loadTR(dtr,uid);
}

function fxf_fn_fxLink(element,href,post)
{
	var url='';
	if(href && href.length)
		url=href;
	else if(element && element.attributes && (typeof element.attributes['href'] != 'undefined'))
		url=element.attributes['href'].value;
	if(url.length)
	{
		var upn=url;
		var usp=url.indexOf('url=');
		if(usp > 0)
			upn=url.substr(usp+4);
		if(upn.substr(0,11) == 'filemanager')
			fxf_fn_fxFilemanager(url)
		else if(upn.substr(0,8) == 'download')
		{
			window.location=url+fxf_fn_gParam();
			fxf_fn_waitScreen('');
		}
		else if((url.substr(0,1) == '?') || (upn.substr(0,1) == '?'))	// main window
		{
			if(url.substr(0,1) == '?')
				var ua1=url.substr(1);
			else
				var ua1=upn.substr(1);
			var dtr=oFXP.tr;
			if(element && element.attributes && (typeof element.attributes['tr'] != 'undefined'))
				dtr=element.attributes['tr'].value;
			if(element)
				fxf_fn_loadTR(dtr,element.uid+'&keep_get=1&'+ua1);
			else
				fxf_fn_loadTR(dtr,ua1);
		}
		else	// new popup transaction
		{
			var qi=url.indexOf('?');
			if(qi < 0)
				url += '?';
			else
				url += '&';
			url += 'no_rdb=1&lng='+oFXP.lang+fxf_fn_gParam();
//alert('fxLink: popup with url='+url);
			var pst='tr='+oFXP.tr;
			// Project data
			if(post && post.length)
				pst += '&'+post;
//alert('pst='+pst);
			new Ajax.Request(url,
			{
				method:'post', postBody:pst, asynchronous:true,
				onSuccess: function(transport)
				{
//fxf_fn_writeDebug('log', transport.responseText);
					fxf_fn_fxLinkDisplay(transport.responseText);
				},
				onFailure: function()
				{
//alert('FAILURE');
					fxf_fn_waitScreen('');
				}
			});
		}
	}
}

function fxf_fn_fxFilemanager(url)
{
	url += fxf_fn_gParam();
//alert('fxf_fn_fxFilemanager:\nurl='+url);
	new Ajax.Request(url,
	{
		method:'post', postBody:'check=1', asynchronous:true,
		onSuccess: function(transport)
		{
//fxf_fn_writeDebug('log', transport.responseText);
			var rt=transport.responseText;
			var rs=rt.split('|');
			var fcs=parseInt(rs[0]);
//alert('Check:\nrt='+rt+'\nrs='+rs+'\n-> fcs='+fcs);
			// Error
			if(!fcs)
			{
				fxf_fn_showMessage('100', rs[1]);
				fxf_fn_waitScreen('');
			}
			// Download
			else if(fcs == 1)
			{
				window.location=url;
				fxf_fn_waitScreen('');
			}
			// Display
			else if(fcs == 2)
			{
				var iframe='<iframe id="fm_display_iframe" src="'+url+'#'+fxf_fn_today(true)+'" width="100%" height="100%" marginwidth="0" marginheight="0" hspace="0" vspace="0" frameborder="0" style="border:0;margin:0;padding:0;"></iframe>';
				var fsw=window.open('','_blank','left='+(200+screen.availLeft)+',top=100,width='+(dim.sd.pwidth-400)+',height='+(dim.sd.pheight-200)+',toolbar=no,location=no,directories=no,status=no,menubar=no,scrollbars=yes,copyhistory=no,resizable=yes');
				fsw.document.write('<html><head><title>'+rs[1]+'</title></head><body>'+iframe+'</body></html>');
				fxf_fn_waitScreen('');
				fsw.focus.delay(oFXP.sdelay);
			}
			else
			{
				var iframe='<iframe id="fm_display_iframe" src="'+url+'#'+fxf_fn_today(true)+'" width="'+(dim.sd.pwidth-400)+'" height="'+(dim.sd.pheight-200)+'" marginwidth="0" marginheight="0" hspace="0" vspace="0" frameborder="0" style="border:0;margin:0;padding:0;"></iframe>';
				fxf_fn_fxLinkDisplay(iframe, rs[1]);
			}
		},
		onFailure: function()
		{
//alert('FAILURE');
			fxf_fn_waitScreen('');
		}
	});
}

function fxf_fn_actionColumn(act,ap)
{
//alert('fxf_fn_actionColumn('+act+','+ap+')');
	fxf_fn_waitScreen('loading');

	var lp='actreg'+act;

	$('actreg1').className='actregi';
	$('actreg2').className='actregi';
	$('actreg3').className='actregi';
	$('actreg4').className='actregi';
	$(lp).className='actrega';

	if(ap)
		lp += '&aaid='+ap;
	fxf_fn_loadTR(oFXP.tr,lp);
}

function fxf_fn_getFirstChar(s,toup)
{
	var c='';
	if(s.length)
	{
		c=s.substr(0,1);
		if(toup)
			c=c.toUpperCase();
	}

	return c;
}

function fxf_fn_taskMsg(msg)
{
	var m=msg.substr(0,1);
	if(m == '+')
	{
		if((msg.length > 2) && (msg.substr(1,1) != ' '))
			oID.fxtaskt.innerHTML += ', '+msg.substr(1);
		else
			oID.fxtaskt.innerHTML += msg.substr(1);
	}
	else
		oID.fxtaskt.innerHTML=msg;
}

function fxf_fn_resetChanged()
{
	oFXP.busy=0.0;
	oFXP.changed=0;
	oFXP.addchg=false;
}

function fxf_fn_keepUserAlive(a)
{
	if(oSet)
	{
		var c=parseInt(oSet.client);
		if(c > 0 )
		{
			var m=0;
			if(oFXP.tr == 281)
			{
				var mels=$$('[id^="pci_smp_"]');
				if(mels.length)
				{
					for(var i=0; i<mels.length; i++)
						m += parseInt(mels[i].id.substr(8));
				}
			}
			if(!oFXP.busy)
			{
				var url=fxf_fn_gProgram('users', 'kua=1&c='+c+'&u='+parseInt(oSet.user)+'&p='+parseInt(oSet.person)+'&t='+oFXP.tr+'&l='+oFXP.lts+'&n='+fxf_fn_escapeText(oSet.pname,false)+'&a='+a+'&m='+m+fxf_fn_gParam());
//alert('a='+a+'\nurl='+url);
				new Ajax.Request(url,
				{
					method:'get', asynchronous:true,
					onSuccess: function(transport)
					{
						var r=transport.responseText;
						if(r.length)
						{
//alert('keepUserAlive('+a+')\nurl='+url+'\n\n'+r);
							if(r.substr(0,4) == '#PL#')
								fxf_fn_prjCheck(r.substr(4));
							else if(r.substr(0,4) == '#RL#')
							{
								fxf_fn_waitScreen('reloading');
								fxf_fn_loadTR.delay(oFXP.ldelay, oFXP.tr,'Reload');
							}
						}
					}
				});
			}
			if((jsKUA > 0.0) && (oFXP.tr > 0) && !a.length)
				fxf_fn_keepUserAlive.delay(jsKUA, '');
		}
	}
}

function fxf_fn_reload()
{
	$('lts').value=oFXP.lts;
	$('fxhaf').submit();
}

function fxf_fn_setPageStatus(c)
{
	if(oID.fxtasksi && oID.fxtasksh)
	{
		var pp=oID.fxtasksi.src.indexOf('pcs_');
		var ns=oID.fxtasksi.src.substr(0,pp+4)+c+'.png';
//alert('fxf_fn_setPageStatus\nc='+c+'\nsrc='+oID.fxtasksi.src+'\nns='+ns);

		var ll=oFXP.lang;
		var llang=$('log_lang');
		if(llang)
			ll=parseInt(llang.innerHTML);

		oID.fxtasksi.src=ns;
		oID.fxtasksh.value=c;
		if(c == 'green')
		{
			if(ll == 1)
				oID.fxtasksi.attributes['tooltip'].value='Seitenstatus: Neu';
			else
				oID.fxtasksi.attributes['tooltip'].value='Page status: New';
		}
		else if(ll == 1)
			oID.fxtasksi.attributes['tooltip'].value='Seitenstatus: Bearbeitet';
		else
			oID.fxtasksi.attributes['tooltip'].value='Page status: Edited';
	}
}

function fxf_fn_stripTags(input, allowed)
{
	allowed = (((allowed || '') + '').toLowerCase().match(/<[a-z][a-z0-9]*>/g) || []).join('');

	var tags = /<\/?([a-z][a-z0-9]*)\b[^>]*>/gi;
	var commentsAndPhpTags = /<!--[\s\S]*?-->|<\?(?:php)?[\s\S]*?\?>/gi;

	return input.replace(commentsAndPhpTags, '').replace(tags, function($0, $1) {
		return allowed.indexOf('<' + $1.toLowerCase() + '>') > -1 ? $0 : ''
	});
}

function fxf_fn_getScreenDimensions()
{
	var sd={};

	sd.swidth=screen.width;
	sd.sheight=screen.height;

	sd.awidth=screen.availWidth;
	sd.aheight=screen.availHeight;

	var e=window;
	var a='inner';
	if(!('innerWidth' in window))
	{
		a='client';
		e=document.documentElement || document.body;
	}
	sd.pwidth=e[a+'Width'];
	sd.pheight=e[a+'Height'];
//alert('ScreenDimensions:\ns='+sd.swidth+'x'+sd.sheight+'\na='+sd.awidth+'x'+sd.aheight+'\np='+sd.pwidth+'x'+sd.pheight);

	return sd;
}

function fxf_fn_getBCR(element)
{
	var d=element.getBoundingClientRect();

	var r={};
	r.left=parseInt(d.left);
	r.right=parseInt(d.right);
	r.top=parseInt(d.top);
	r.bottom=parseInt(d.bottom);
	r.width=d.right-d.left;
	r.height=d.bottom-d.top;

	return r;
}

function fxf_fn_getDimensions()
{
	dim={'sd':fxf_fn_getScreenDimensions(), 'wd':fxf_fn_getBCR(oID.fxwork)};
//alert('dim:\n\n.sd: s='+dim.sd.swidth+'x'+dim.sd.sheight+', a='+dim.sd.awidth+'x'+dim.sd.aheight+', p='+dim.sd.pwidth+'x'+dim.sd.pheight+'\n\n.wd: left='+dim.wd.left+', top='+dim.wd.top+', right='+dim.wd.right+', bottom='+dim.wd.bottom+' -- width='+dim.wd.width+', height='+dim.wd.height);
}

function fxf_fn_resize()
{
	// Body dimensions and scrolls offsets
	fxf_fn_getDimensions();

	// Text Popup?
	if(oID.txtpoptoggle.style.display != 'none')
		oID.txtpoptoggle.style.display='none';

	// Adjust toolbar?
	var te=$$('[class="ticontxt"]');
	if(te.length > 0)
	{
		for(var t=0; t<te.length; t++)
			te[t].style.display='';

		var tl=$('fxswlogo');
		tl.style.display='';

		var th=$('fxhlp');
		if(oFXP.lang)
			th.style.display='';

		var tf=$('fxfltseld');
		var ts=$('fxsrc');
		if(oFXP.tr && oFXP.lang)
		{
			tf.style.display='';
			ts.style.display='';
		}

		var td=fxf_fn_getBCR(oID.fxtooli);
		if((td.left < 64) && oFXP.lang)
		{
			for(var t=0; t<te.length; t++)
			{
				td=fxf_fn_getBCR(oID.fxtooli);
				if(td.left < 64)
					te[t].style.display='none';
			}
			td=fxf_fn_getBCR(oID.fxtooli);
			if(td.left < 64)
				tl.style.display='none';
			td=fxf_fn_getBCR(oID.fxtooli);
			if(td.left < 4)
				ts.style.display='none';
			td=fxf_fn_getBCR(oID.fxtooli);
			if(td.left < 4)
				tf.style.display='none';
		}
	}

	// Adjust menu bars?
	if(oFXP.tr)
	{
		fxf_fn_drawMenu('');
		var m1=fxf_fn_getBCR(oID.fxmenu1c);
		oID.fxmenu1c.style.left=0;
		oID.fxmenu1l.style.display='none';
		oID.fxmenu1r.style.display='none';
		oID.fxmenu2c.style.left=0;
		oID.fxmenu2l.style.display='none';
		oID.fxmenu2r.style.display='none';
		oID.fxmenu3c.style.left=0;
		oID.fxmenu3l.style.display='none';
		oID.fxmenu3r.style.display='none';
		if(oID.fxmenu1.style.display != 'none')
		{
			if(m1.width > dim.sd.pwidth)
				oID.fxmenu1r.style.display='';
		}
	}

	// Adjust breadcrumb, actions etc.
	fxf_fn_resizeCheck();

	// Adjust taskbar
	var tba=['p','t','d','l','u','f','s','c','i'];
	var tbo={};
	for(var t=0; t<tba.length; t++)
	{
		var el=$('fxtask'+tba[t]);
		el.style.display='';
		tbo[tba[t]]={'elem':el, 'dim':fxf_fn_getBCR(el)};

		if(!oFXP.lang && (tba[t] != 't') && (tba[t] != 'i'))
			el.style.display='none';
	}
	if(!oFXP.tr)
	{
		tbo.p.elem.style.display='none';
		tbo.d.elem.style.display='none';
		tbo.u.elem.style.display='none';
		tbo.f.elem.style.display='none';

		tbo.t.elem.style.left=0;
	}
	else
	{
		tbo.t.elem.style.left=tbo.p.dim.width+'px';
	}

	for(var t=0; t<tba.length; t++)
		tbo[tba[t]].dim=fxf_fn_getBCR(tbo[tba[t]].elem);

	var ml=tbo.t.dim.left+480;
	tbo.t.elem.style.width=(tbo.l.dim.left-24-tbo.t.dim.left)+'px';


/*
	var tbg=['i','s','c','d','p'];
	for(var g=0; g<tbg.length; g++)
	{
		tbo.l.dim=fxf_fn_getBCR(tbo.l.elem);
		if(tbo.l.dim.left < ml)
		{
			tbo[tbg[g]].elem.style.display='none';
			if(tbg[g] == 'p')
				tbo.t.elem.style.left=0;
		}
		else
			break;
	}
*/

	tbo.l.dim=fxf_fn_getBCR(tbo.l.elem);
	tbo.t.dim=fxf_fn_getBCR(tbo.t.elem);
	tbo.t.elem.style.width=(tbo.l.dim.left-24-tbo.t.dim.left)+'px';
//fxf_fn_taskMsg('taskbar: t.left='+tbo.t.dim.left+', t.width='+tbo.t.dim.width+' -- l.left='+tbo.l.dim.left+', ml='+ml);

	var pwd=$('pwaitscreen').style.display;

	// Adjust workflow slider
	if(gSet.wfsl > 0)
	{
		var we=fxf_fn_getBCR(oID.iworkflow);
		if(dim.sd.pwidth-64 < we.width)
			oID.iworkflow.style.width=(dim.sd.pwidth-64)+'px';
	}
	else if(oID.iworkflow.style.display != 'none')
		fxf_fn_shrinkWorkflow.delay(1.0, 0.25,0,pwd);

	// Adjust project slider
	var pe=fxf_fn_getBCR(oID.iprojects);
	var pl=parseInt(oID.iprojects.style.left);
	var nl=pl;
	var pw=Math.min(900, dim.sd.pwidth-64);
	oID.iprojects.style.width=pw+'px';
	if((oID.iprojects.style.display != 'none') && (pl < 0))
	{
		if(pl <= -920)
			fxf_fn_shrinkProjects.delay(1.0, 0.25,0,pwd);
		else
		{
			nl=-(pw-2);
			oID.iprojects.style.left=nl+'px';
		}
	}
	var ps=$('pjsmt_src');
	ps.style.width=Math.min(300, pw-218)+'px';
//fxf_fn_taskMsg('projects: pe.width='+pe.width+' -- pl='+pl+', pw='+pw+', nl='+nl);

	$('pwaitscreen').style.display=pwd;

	// Adjust message slider?
	var mw=Math.min(480, dim.sd.pwidth-64);
	var mr=parseInt(oID.imessage.style.right);
	oID.imessage.style.width=mw+'px';
	if(mr < 0)
		oID.imessage.style.right=(3-mw)+'px';

	// Dcoument Manager
	if(((oFXP.tr == 107) || (oFXP.tr == 110)) && tSet.dms && tSet.dms.sdi_container)
	{
		var wt=parseInt(tSet.dms.sdi_container.clientWidth);
		if(wt > 0)
		{
			var ie=$('ienlarge');
			var ws=parseInt(tSet.dms.structure.clientWidth);
			var wd=parseInt(tSet.dms.detail.clientWidth);
			var wi=0;
			if(tSet.dms.ilarge)
				wi=wt-ws-wd;
			else
				wd=wt-ws-20;

			if(!tSet.dms.iwidth)
			{
				if(wi <= 0)
					tSet.dms.iwidth=Math.max(tSet.dms.mig, wd-770);
				else
					tSet.dms.iwidth=wi;
			}

			if((wi > 0) && (wi < tSet.dms.mig))
			{
				wd -= (tSet.dms.mig-wi);
				wi=tSet.dms.mig;
			}
			if(ws && (wd < tSet.dms.mig))
			{
				ws -= (tSet.dms.mig-wd);
				wd=tSet.dms.mig;
			}
			if(ws && (ws < tSet.dms.mig))
			{
				ws=tSet.dms.mig;
				wd=wt-ws-20;
				fxf_fn_dmsAreaSet('Screen(I)', ws,wd,0);
				if(wi > 0)
				{
					wi=0;
					fxf_fn_dmsInfoShrink()
				}
			}
			else
			{
				if(wi > 0)
					wd=wt-ws-wi;
				else
					wd=wt-ws-20;
				fxf_fn_dmsAreaSet('Screen', ws,wd,wi);
			}

			ie.style.display='';
			if(!wi && (wt < 3*tSet.dms.mig))
				ie.style.display='none';

			fxf_fn_dmsButtons();
		}
	}
	// Matrix
	else if((oFXP.tr == 189) && oID.sdg_container)
	{
		// Main container offsets
		var sdg_width=parseInt(oID.sdg_container.style.width);
		if(!sdg_width)
			sdg_width=oID.sdg_container.clientWidth;
		if(!sdg_width)
			sdg_width=dim.sd.pwidth-28;

		// Project structure and detail width
		var swidth=16;
		if(slarge)
		{
			oID.sstructure.style.display='none';
			oID.structure.style.display='';

			swidth=parseInt(oID.structure.style.width);
			if(!swidth)
				swidth=oID.structure.clientWidth;
			if(!swidth)
				swidth=256;

			if(hlarge)
				oID.hrnames.style.width=swidth+'px';

			if(oID.pprefix)
				oID.pprefix.style.width=(swidth-58)+'px';

			if(dlarge || glarge)
				oID.sd_divider.style.display='';
			else
				oID.sd_divider.style.display='none';
		}
		else
		{
			oID.sstructure.style.display='';
			oID.structure.style.display='none';

			oID.sd_divider.style.display='none';
		}
		swidth += 1;	// 1px border right

		var dwidth=16;
		if(dlarge)
		{
			oID.sdetail.style.display='none';
			oID.detail.style.display='';

			dwidth=parseInt(oID.detail.style.width);
			if(!dwidth)
				dwidth=oID.detail.clientWidth;
			if(!dwidth)
				dwidth=423;

			if(hlarge)
				oID.hrdata.style.width=dwidth+'px';

			if(glarge)
				oID.dg_divider.style.display='';
			else
				oID.dg_divider.style.display='none';
		}
		else
		{
			oID.sdetail.style.display='';
			oID.detail.style.display='none';

			oID.dg_divider.style.display='none';
		}
		dwidth += 1;	// 1px border right

		// Set new width
		if(glarge)
		{
			oID.sgantt.style.display='none';

			var gwidth=sdg_width-swidth-dwidth;
			oID.gantt.style.width=gwidth+'px';
			if(hlarge)
				oID.hrworkload.style.width=gwidth+'px';
		}
		else
		{
			oID.sgantt.style.display='';

			var gwidth=33;
			oID.gantt.style.width=gwidth+'px';
			if(hlarge)
				oID.hrworkload.style.width=gwidth+'px';

			if(dlarge)
			{
				dwidth=sdg_width-swidth-gwidth;
				oID.detail.style.width=dwidth+'px';
				if(hlarge)
					oID.hrdata.style.width=dwidth+'px';
			}
			else
			{
				swidth=sdg_width-dwidth-gwidth;
				oID.structure.style.width=swidth+'px';
				if(hlarge)
					oID.hrnames.style.width=swidth+'px';
			}
		}

		// Set new left
		oID.detail.style.left=(swidth+1)+'px';
		oID.sdetail.style.left=(swidth+1)+'px';
		oID.gantt.style.left=(swidth+dwidth+2)+'px';
		oID.sgantt.style.left=(swidth+dwidth+2)+'px';
		if(hlarge)
		{
			oID.hrdata.style.left=(swidth+1)+'px';
			oID.hrworkload.style.left=(swidth+dwidth+2)+'px';
			oID.hrpool.style.left=(swidth+3)+'px';
		}
//alert('sdg_width='+sdg_width+' -- swidth='+swidth+' + dwidth='+dwidth+' + gwidth='+gwidth+' = '+(swidth+dwidth+gwidth));

		// Set bottom
		if(oID.ndw_container)
		{
			oID.hrbplan.style.width=(swidth+dwidth)+'px';
			oID.hrbwload.style.width=gwidth+'px';

			var hh=0;
			var hrbar=$('hrbar');
			var hrbplit=$('hrbplit');
			var hrbpset=$('hrbpset');
			var hrbwlit=$('hrbwlit');
			var hrbwset=$('hrbwset');
			if(hlarge)
			{
				hh=Math.max(hrheight,124);
				hrheight=hh;
				hrbar.className='darkgrey bold';
				$('hrenlarge').style.display='none';
				$('hrshrink').style.display='';
				$('hrenlarget').style.display='none';
				$('hrshrinkt').style.display='';
				oID.mh_divider.style.display='';
				oID.overview.style.display='none';
				hrbplit.style.display='';
				hrbplit.style.width=(swidth+dwidth-46)+'px';
				hrbpset.style.display='';
				if(gwidth > 410)
				{
					hrbwlit.style.display='';
					hrbwlit.style.width=(gwidth-236)+'px';
				}
				else
					hrbwlit.style.display='none';
				hrbwset.style.display='';
			}
			else
			{
				hh=18;
				hrbar.className='lightergrey bold';
				$('hrenlarge').style.display='';
				$('hrshrink').style.display='none';
				$('hrenlarget').style.display='';
				$('hrshrinkt').style.display='none';
				oID.mh_divider.style.display='none';
				oID.overview.style.display='';
				hrbplit.style.display='';
				hrbplit.style.width=(swidth+dwidth)+'px';
				hrbpset.style.display='none';
				hrbwlit.style.display='';
				hrbwlit.style.width=gwidth+'px';
				hrbwset.style.display='none';
			}

			oID.ndw_container.style.height=(hh+3)+'px';
			oID.structure.style.bottom=hh+'px';
			oID.sstructure.style.bottom=hh+'px';
			oID.detail.style.bottom=hh+'px';
			oID.sdetail.style.bottom=hh+'px';
			oID.gantt.style.bottom=hh+'px';
			oID.sgantt.style.bottom=hh+'px';

			var gh=oID.gmain.clientHeight;
			if(glarge && (gh > 262))
			{
				oID.overview.style.display='';
				var oh=parseInt(oID.overview.style.height);
				var bh=58;
				if(Prototype.BrowserFeatures['Mobile'].length)
					bh -= 18;
				if(oh >= 240)
					oID.overview.style.bottom=(bh+2+hh)+'px';
				else
					oID.overview.style.bottom=(bh+hh)+'px';
			}
			else
				oID.overview.style.display='none';
		}

		// Hide shrink icons, if two areas are already shrunked or hr area active
		var sic=0;
		if(hlarge)
			sic=3;
		else
		{
			if(!slarge)
				sic++;
			if(!dlarge)
				sic++;
			if(!glarge)
				sic++;
		}
		if(sic > 1)
		{
			oID.sshrink.style.display='none';
			oID.dshrink.style.display='none';
			oID.gshrink.style.display='none';
		}
		else
		{
			oID.sshrink.style.display='';
			oID.dshrink.style.display='';
			oID.gshrink.style.display='';
		}

		if(pArray.length && pArray[0].celement)
		{
			mlines=0;
			for(var pc=0; pc<pArray.length; pc++)
			{
				if(pArray[pc].celement && (pArray[pc].celement.style.display == ''))
					mlines++;
			}
		}
		else
			mlines=pArray.length;
//alert('mdays='+mdays+', mlines='+mlines);

		// Project gantt main dimensions
		mdays=1+fxf_fn_diffDays(sdate,fxf_fn_getEDate());

		var gheight=oID.gbars.clientHeight;
		if(!gheight)
			gheight=dim.sd.pheight-320;
		var nlines=mlines+Math.floor((dim.sd.pheight-168)/dh);
		mlines=Math.max(nlines,Math.floor(gheight/dh));

		var bwidth=mdays*zdays[oFXP.zoom];
		var bheight=mlines*dh;
//alert('mdays='+mdays+', mlines='+mlines+' -- nlines='+nlines+' -- gwidth='+gwidth+', bwidth='+bwidth+' -- gheight='+gheight+', bheight='+bheight);

		if(glarge)
		{
			oID.gtime.style.width=(gwidth+16)+'px';
			oID.time0.style.width=(bwidth+40)+'px';
			oID.time1.style.width=(bwidth+40)+'px';
			oID.time2.style.width=(bwidth+40)+'px';
			oID.gbars.style.width=bwidth+'px';

			if(hlarge)
			{
				oID.wtime.style.width=(gwidth+16)+'px';
				oID.wtime2.style.width=(bwidth+zdays[oFXP.zoom])+'px';
				oID.wbars.style.width=bwidth+'px';
			}
		}
		oID.swork.style.height=bheight+'px';
		oID.dwork.style.height=bheight+'px';
		oID.gbars.style.height=bheight+'px';

		fxf_fn_HRHeight();

		// Remember width
		if(slarge)
			srwidth=parseInt(oID.structure.style.width);
		if(dlarge)
			drwidth=parseInt(oID.detail.style.width);
		if(glarge)
			grwidth=parseInt(oID.gantt.style.width);

		fxf_fn_markObject(false);
		fxf_fn_drawOverview();
	}
}

function fxf_fn_resizeCheck()
{
	if(!oFXP.tr)
		return;

	// Adjust breadcrumb, actions, header + buttons?
	var abd=oID.fxtaction.style.display;
	var ht=$('fxheadert');
	oID.fxpath.style.display='';
	oID.fxtaction.style.display='';
	oID.fxbuttons.style.display='';
	ht.style.display='';

	var aa=null;
	for(var a=1; a<5; a++)
	{
		aa=$('actreg'+a);
		aa.innerHTML=aa.attributes['ll'].value;
		aa.style.width='110px';
	}
	oID.fxbuttons.style.right='64px';
	var pd=fxf_fn_getBCR(oID.fxpath);
	var ad=fxf_fn_getBCR(oID.fxtaction);
	if(ad.left < pd.right+8)
	{
		for(a=1; a<5; a++)
		{
			aa=$('actreg'+a);
			aa.innerHTML=aa.attributes['ls'].value;
			aa.style.width='56px';
		}
//		oID.fxbuttons.style.right='54px';
	}
	ad=fxf_fn_getBCR(oID.fxtaction);
	if(ad.left < pd.right+8)
		oID.fxpath.style.display='none';
//fxf_fn_taskMsg('actions: pd.right='+pd.right+' | ad.left='+ad.left);

	// Adjust header + buttons?
	var hd=fxf_fn_getBCR(ht);
	var bd=fxf_fn_getBCR(oID.fxbuttons);
	if(bd.left < hd.right)
	{
		ht.style.display='none';
	}
//fxf_fn_taskMsg('header/buttons: hd.right='+hd.right+' | bd.left='+bd.left);

	oID.fxtaction.style.display=abd;
}

function fxf_fn_setMenu(txt, mtr)
{
	if(mtr === undefined)
		mtr=oFXP.tr;
	else
		mtr=parseInt(mtr);
//alert('fxf_fn_setMenu(txt, mtr='+mtr+')');
	// Remove old definitions?
	if(txt.length)
	{
		var ia=['mstructure','mentries','mcolors','tpath'];
		for(var i=0; i<ia.length; i++)
		{
			var did=document.getElementById(ia[i]);
//alert('Remove #'+i+': "'+ia[i]+'" ... did='+did);
			if(did)
			{
				var sp=did.parentNode;
				sp.removeChild(did);
			}
		}

		// ...Add new menu definitions
		var did=document.createElement('div');
		did.id='menu_definitions';
		did.innerHTML=txt;
		document.body.appendChild(did);
//alert('New menu definition:\n\n'+did.innerHTML);
	}

	var mstructure=$('mstructure');
	if(mstructure)
	{
		oFXP.mstructure=[];
		oID.fxmenu1.style.display='none';
		oID.fxmenu2.style.display='none';
		oID.fxmenu3.style.display='none';
		var mentries=mstructure.innerHTML.split('~');
		mstructure.outerHTML='';
//alert('New menu structure: Found '+mentries.length+' entries');
		if(mentries.length)
		{
			var mec=0;
			for(var i=0;i<mentries.length;i++)
			{
				var medata=mentries[i].split('|');
				if(medata.length > 4)
				{
					oFXP.mstructure[mec]={};
					oFXP.mstructure[mec].id=medata[0];
					oFXP.mstructure[mec].lvl=parseInt(medata[1]);
					oFXP.mstructure[mec].tr=parseInt(medata[2]);
					oFXP.mstructure[mec].txt=medata[3];
					oFXP.mstructure[mec].arg=medata[4];
					if(medata.length > 5)
						oFXP.mstructure[mec].tooltip=medata[5];
					mec++;
				}
			}
		}

		fxf_fn_drawMenu('');

		if(!txt.length)
			fxf_fn_taskMsg('+ - Menu set');
	}

	// Menu main entries
	var mentries=$('mentries');
	if(mentries)
	{
		oFXP.mentries=parseInt(mentries.innerHTML);
//alert('New menu main entries: '+oFXP.mentries);

		if(!txt.length)
			fxf_fn_taskMsg('+ - Menu entries set');
	}

	// Menu colors
	var mcolors=$('mcolors');
	if(mcolors)
	{
		// ...Remove old menu colors style sheet
		fxf_fn_resetDesign(false,true);
		// ...and add new menu colors style shett
		var did=document.createElement('style');
		did.id='menu_colors_sheet';
		did.innerHTML=mcolors.innerHTML;
		document.body.appendChild(did);
//alert('New menu color style sheet:\n\n'+did.innerHTML);

		if(!txt.length)
			fxf_fn_taskMsg('+ - Menu colors set');
	}

	// Breadcrumb path
	var tpath=$('tpath');
	if(tpath && oID.fxpath)
	{
		oID.fxpath.innerHTML=tpath.innerHTML;
//alert('New breadcrumb path:\n\n'+oID.fxpath.innerHTML);
	}

	// Menu top border
	var bid=fxf_fn_drawMenuBorder('',mtr);

	if(txt.length && oID && oID.iainfo)
	{
		if(oID.iworkflowi)
		{
			oID.iworkflowi.innerHTML='<table width=100% height=100%><tr><td width=100% height=100% align=center><font class=white>loading&hellip;</font></td></tr></table>';
			gSet.wfradius=0;
		}
		fxf_fn_waitScreen('');
		oID.iainfo.style.display='';
	}
}

function fxf_fn_setFocus(e, sel)
{
//alert('fxf_fn_setFocus(e='+e+', sel='+sel);
	if(e)
	{
		e.focus();
		if(sel)
			e.select();
	}
}


function fxf_fn_toggleScreenshotMode(ts)
{
	var ots=ts;
	if((ts < 0) || ((ts == 0) && (scrshtmode > 0)))
		scrshtmode=-1;
	else if((ts > 0) || ((ts == 0) && (scrshtmode < 0)))
		scrshtmode=1;

	var fxframe=$('fxframe');
	var smtxt=$('fxscrshtmdtxt');
	if(scrshtmode <= 0)
	{
		smtxt.innerHTML='Off';
		var pdisp='none';
		var pcol='';
		oID.fxmain.style.background='';
		oID.fxwork.style.background='';
		oID.fxworkf.style.background='';
		oID.iprojects.style.display='';
		oID.iworkflow.style.display='';
 		oID.waitscreen.style.top='0';

		if(fxframe)
		{
			fxframe.style.width='';
			fxframe.style.height='';
			fxframe.style.right='8px';
			fxframe.style.bottom='8px';
	 		oID.waitscreen.style.right='0';
	 		oID.waitscreen.style.width='';
		}
	}
	else
	{
		smtxt.innerHTML='On';
		var pdisp='';
		var pcol='#969696';
		oID.fxmain.style.background='#ffffff';
		oID.fxwork.style.background='#ffffff';
		oID.fxworkf.style.background='#ffffff';
		oID.iprojects.style.display='none';
		oID.iworkflow.style.display='none';
 		oID.waitscreen.style.top='48px';

		if(fxframe)
		{
			fxframe.style.right='';
			fxframe.style.bottom='';
			fxframe.style.width='1280px';
			fxframe.style.height='840px';
	 		oID.waitscreen.style.right='';
	 		oID.waitscreen.style.width='1290px';
		}
	}

	var tels=$$('.ttitle');
	if(tels.length)
	{
		for(var tc=0; tc<tels.length; tc++)
			tels[tc].style.color=pcol;
	}

	fxf_fn_toggleScreenshotIcons(pdisp);
	var esize=fxf_fn_setWorkMargin();

	if(!ots)
		fxf_fn_resize();
}

function fxf_fn_toggleScreenshotIcons(pdisp)
{
	var odisp='';
	if(pdisp != 'none')
		odisp='none';

	var pels=$$('[id^="fxphoto_"]');
	if(pels.length)
	{
		for(var pc=0; pc<pels.length; pc++)
			pels[pc].style.display=pdisp;
	}

	var ims=$('imswitch');
	if(ims)
		ims.style.display=odisp;
}

function fxf_fn_takeMaskScreenshot(mnr)
{
	var vb_imessage_display=oID.imessage.style.display;
	var vb_fxpath_display=oID.fxpath.style.display;
	var vb_fxtaction_display=oID.fxtaction.style.display;
	var vb_fxheaderc_top=parseInt(oID.fxheaderc.style.top);
	var vb_fxworkf_top=parseInt(oID.fxworkf.style.top);

	var mpi=$('fxphoto_'+mnr);
	if(mnr == 'frame')
	{
		var sfm=mpi.attributes['tooltip'].value.substr(mpi.attributes['tooltip'].value.length-6);
		var ifm=parseInt(sfm);
		if(ifm > 0)
			mnr=ifm;
//alert('Frame tooltip: '+mpi.attributes['tooltip'].value+'\n\nsfm='+sfm+' | ifm='+ifm+'\n\n-> mnr='+mnr);
		fxf_fn_adjustMaskScreenshot('none','none','none',0,vb_fxworkf_top-vb_fxheaderc_top, 'Click','none');

		if(oID.fxworkframe)
			var mfr=oID.fxworkframe;
		else
			var mfr=oID.fxframe;
		var mdi=fxf_fn_getBCR(mfr);
		mdi.height -= vb_fxheaderc_top;
		var html='<div style="width:'+mdi.width+'px;height:'+mdi.height+'px;overflow:hidden;">'+mfr.innerHTML+'</div>';
	}
	else
	{
		fxf_fn_adjustMaskScreenshot('none',vb_fxpath_display,vb_fxtaction_display,vb_fxheaderc_top,vb_fxworkf_top, 'Click','none');

		var mfr=$('mt'+mnr+'_rahmen');
		var mdi=fxf_fn_getBCR(mfr);
		var html=mfr.outerHTML;
	}

	var mtr=oFXP.tr;
	var mlang=oFXP.lang;
	if(oFXP.tr == 121)
	{
		mtr=parseInt($('mt_tr').value);
		mlang=parseInt($('mt_spr').value);
	}

	var cnr=0;
	if(oSet && oSet.client)
		cnr=oSet.client;
	var url=fxf_fn_gProgram('savemask', 'tr='+mtr+'&mnr='+mnr+'&lang='+oFXP.lang+'&cnr='+cnr+'&width='+Math.ceil(mdi.width)+'&height='+Math.ceil(mdi.height)+fxf_fn_gParam());
	var pst='html='+fxf_fn_escapeText(html,false);
//alert('fxf_fn_takeMaskScreenshot('+mnr+')\nurl='+url+'\npst=\n'+pst);

	new Ajax.Request(url,
	{
		method:'post', postBody:pst, asynchronous:true,
		onSuccess: function(transport)
		{
			var rt=transport.responseText;
			var sp=rt.indexOf('^^');
			if(sp >= 0)
			{
//				fxf_fn_fxLinkDisplay('<div style="padding:8px;background:#fff;border:1px solid #000;border-radius:6px;"><b>savemask</b>:<hr size="1" /><br />'+rt+'</div>');
				rt=rt.substring(sp+2);
//alert('rt:\n'+rt);
			}
			mpi.src=rt;
			fxf_fn_adjustMaskScreenshot(vb_imessage_display,vb_fxpath_display,vb_fxtaction_display,vb_fxheaderc_top,vb_fxworkf_top, '','');
		},
		onFailure: function()
		{
			fxf_fn_adjustMaskScreenshot(vb_imessage_display,vb_fxpath_display,vb_fxtaction_display,vb_fxheaderc_top,vb_fxworkf_top, '','');
		}
	});
}

function fxf_fn_adjustMaskScreenshot(imessage_display,fxpath_display,fxtaction_display,fxheaderc_top,fxworkf_top, ws_text, icons)
{
	oID.imessage.style.display=imessage_display;
	oID.fxpath.style.display=fxpath_display;
	oID.fxtaction.style.display=fxtaction_display;
	oID.fxheaderc.style.top=fxheaderc_top+'px';
	oID.fxworkf.style.top=fxworkf_top+'px';

	fxf_fn_waitScreen(ws_text);
	fxf_fn_toggleScreenshotIcons(icons);

	var ft=$('fxfooter_timecheck');
	if(ft)
		ft.style.display=icons;
}

function fxf_fn_setWorkMargin()
{
	var esize=0;

	var cont=null;
	var cleft=16;
	var ctop=8;
	var cright=8;
	var cbottom=4;

	var fleft=-1;
	var ftop=-1;
	var fright=-1;
	var fbottom=-1;

	if(oFXP.tr == 189)	// Matrix
		cright=2;

	if(oID.imessage.style.display != 'none')
		cright += 8;

	oID.trw_container=$('trw_container');
	oID.sdi_container=$('sdi_container');
	oID.sdg_container=$('sdg_container');
	oID.ndw_container=$('ndw_container');
	oID.hb_container=$('hb_container');

	if((oFXP.tr == 83) && oID.trw_container) 	// TR (Week/Month)
	{
		esize=1;
		cont=oID.trw_container;
		cbottom=5;
		var wpd=$('warn_pers_date');
		if(wpd)
			ctop=115;
	}
	else if(((oFXP.tr == 107) || (oFXP.tr == 110)) && oID.sdi_container) 	// Search Documents || DMS
	{
		esize=1;
		cont=oID.sdi_container;
		ctop=36;
		ftop=36;
	}
	else if((oFXP.tr == 189) && oID.sdg_container)	// Matrix
	{
		esize=1;
		cont=oID.sdg_container;
		ctop=36;
		ftop=36;
	}
	else if(oFXP.tr == 231)	// Manual
	{
		esize=1;
		cont=oID.hb_container;
		cright=2;
		fright=2;
	}

	if(cont && (scrshtmode == 1))
	{
		if(fleft >= 0)
			cleft=fleft;
		else
			cleft=2;

		if(ftop >= 0)
			ctop=ftop;
		else
			ctop=2;

		if(fright >= 0)
			cright=fright;
		else
			cright=2;

		if(fbottom >= 0)
			cbottom=fbottom;
		else
			cbottom=2;
	}
//alert('tr='+oFXP.tr+' - esize='+esize+' - scrshtmode='+scrshtmode+'\ncont='+cont+' (left='+cleft+', top='+ctop+', right='+cright+', bottom='+cbottom+')');

	if(esize == 1)
	{
		oID.fxwork.style.left='0';
		oID.fxwork.style.top='0';
		oID.fxwork.style.right='0';
		oID.fxwork.style.bottom='0';

		oID.fxwork.style.overflowX='hidden';
		oID.fxwork.style.overflowY='hidden';

		if(cont)
		{
			cont.style.left=cleft+'px';
			cont.style.top=ctop+'px';
			cont.style.right=cright+'px';
			cont.style.bottom=cbottom+'px';

			var abd=$('actionbar');
			if(abd)
			{
				abd.style.left=cleft+'px';
				abd.style.right=cright+'px';
			}

			var msg=$('sdg_message');
			if(msg)
			{
				msg.style.left=cleft+'px';
				msg.style.right=cright+'px';
			}

			if(oID.ndw_container)
			{
				oID.ndw_container.style.left=cleft+'px';
				oID.ndw_container.style.right=cright+'px';
			}
		}
	}
	else
	{
		oID.fxwork.style.left=cleft+'px';
		oID.fxwork.style.top=ctop+'px';
		oID.fxwork.style.right=cright+'px';
		oID.fxwork.style.bottom=cbottom+'px';

		oID.fxwork.style.overflowX='auto';
		oID.fxwork.style.overflowY='scroll';
	}

	return esize;
}

function fxf_fn_getType(val)
{
	if(typeof val == 'number')
	{
		if(Math.ceil(parseFloat(val)) === val)
			return 'I';
		return 'F';
	}
	else
		return 'S';
}

function fxf_fn_getTypeValue(val)
{
	return fxf_fn_getType(val)+''+val;
}

function fxf_fn_setOSet(key,value,o)
{
	var type=value.substr(0,1);
	value=value.substr(1);

	if(type == 'I')			// Integer
		value=parseInt(value);
	else if(type == 'F')	// Float
		value=parseFloat(value);
	else
		value=fxf_fn_escapeText(value,true);

	if(key.length)
	{
		if(o == 't')
			tSet[key]=value;
		else
			oSet[key]=value;
	}

	return value;
}

function fxf_fn_docDownload(idx)
{
	window.location=fxf_fn_gProgram('download', 'doctr='+oFXP.tr+'&docidx='+idx+fxf_fn_gParam());
}

function fxf_fn_calHolidays()
{
	if(hArray.length)
	{
		for(var hc=0; hc<hArray.length; hc++)
		{
			hArray[hc].offset=fxf_fn_diffDays(sdate, hArray[hc].date);
//alert(hc+': '+hArray[hc].date+' - '+hArray[hc].name+' ['+hArray[hc].type+']='+hArray[hc].offset);
		}
	}
}

function fxf_fn_getMessage(msg, headline)
{
	var txt='';
	switch(msg)
	{
		case 1:	// Copy here
			if(oFXP.lang == 1)
				txt='Hierhin <b>kopieren</b>';
			else
				txt='<b>Copy</b> here';
		break;

		case 2:	// Move here
			if(oFXP.lang == 1)
				txt='Hierhin <b>verschieben</b>';
			else
				txt='<b>Move</b> here';
		break;

		case 3:	// A main project cannot be deleted!
			if(oFXP.lang == 1)
				txt='Ein Hauptprojekt darf nicht gelöscht werden!';
			else
				txt='A main project cannot be deleted!';
		break;

		case 4: // Invalid dependencies
			if(oFXP.lang == 1)
				txt='<b style="color:#e1001a;">Ungültige Abhängigkeitsbeziehung!</b>|Ungütige Abhängigkeitsbeziehung löschen';
			else
				txt='<b style="color:#e1001a;">Invalid dependencies!</b>|Delete invalid dependencies';
		break;

		case 5: // Unsaved Data - Really Leave?
			if(oFXP.lang == 1)
				txt='Ungespeicherte Daten - Wirklich verlassen?|<font class="red">Es wurden Daten eingegeben bzw. verändert, welche noch nicht gespeichert wurden!</font><br /><br /><b>Wollen Sie wirklich diese Programmfuntkion verlassen ohne die Daten zu speichern?</b>';
			else
				txt='Unsaved Data - Really Leave?|<font class="red">Data has been inputed or changed that has not been saved yet!</font><br /><br /><b>Do you really want to leave this program function without saving your data?</b>';
		break;

		case 6: // Language: Unsaved Data - Really Reload?
			if(oFXP.lang == 1)
				txt='Sprache: Ungespeicherte Daten - Wirklich neu laden?|<font class="red">Es wurden Daten eingegeben bzw. verändert, welche noch nicht gespeichert wurden!</font><br /><br /><b>Wollen Sie wirklich diese Seite in einer anderen Sprache<br />neu laden ohne die Daten vorher zu speichern?</b>';
			else
				txt='Language: Unsaved Data - Really Reload?|<font class="red">Data has been inputed or changed that has not been saved yet!</font><br /><br /><b>Do you really want to reload this page in another language<br />without saving your data first?</b>';
		break;

		case 7: // Filter: Unsaved Data - Really Reload?
			if(oFXP.lang == 1)
				txt='Filter: Ungespeicherte Daten - Wirklich neu laden?|<font class="red">Es wurden Daten eingegeben bzw. verändert, welche noch nicht gespeichert wurden!</font><br /><br /><b>Wollen Sie wirklich diese Seite mit neuen Filtereinstellungen<br />erneut laden ohne die Daten vorher zu speichern?</b>';
			else
				txt='Filter: Unsaved Data - Really Reload?|<font class="red">Data has been inputed or changed that has not been saved yet!</font><br /><br /><b>Do you really want to reload this page with new filter settings<br />without saving your data first?</b>';
		break;

		case 8: // Release Field?
			if(oFXP.lang == 1)
				txt='Feld freigeben?|Dieses Feld wirklich zum <b>Ändern</b> freigeben?';
			else
				txt='Release Field?|Really release this field for <b>changing</b>?';
		break;

		case 9:
			if(oFXP.lang == 1)
				txt='Doppelklick zum Umnummerieren des Vorgangs';
			else
				txt='Double click to renumber process';
		break;

		case 10:
			if(oFXP.lang == 1)
				txt='<b style="color:#e1001a;">Budgetrahmenüberschreitung!</b>|Budgetrahmen anpassen|Aktion rückgängig machen|Projekt|Internes Budget|Externes Budget|PB|MB';
			else
				txt='<b style="color:#e1001a;">Budget frame exceedance!</b>|Adjust budget frames|Undo last action|Project|Internal Budget|External Budget|HB|MB';
		break;

		case 11: // Action Mode: Unsaved Data - Really Reload?
			if(oFXP.lang == 1)
				txt='Aktionsmodus: Ungespeicherte Daten - Wirklich neu laden?|<font class="red">Es wurden Daten eingegeben bzw. verändert, welche noch nicht gespeichert wurden!</font><br /><br /><b>Wollen Sie wirklich den Aktionsmodus wechseln oder erneut aufrufen ohne die Daten vorher zu speichern?</b>';
			else
				txt='Action Mode: Unsaved Data - Really Reload?|<font class="red">Data has been inputed or changed that has not been saved yet!</font><br /><br /><b>Do you really want to change or reload the action mode without saving your data first?</b>';
		break;

		case 12: // Popup Program Function: Unsaved Data - Really Execute?
			if(oFXP.lang == 1)
				txt='Popup-Programmfunktion: Ungespeicherte Daten - Wirklich ausführen?|<font class="red">Es wurden Daten eingegeben bzw. verändert, welche noch nicht gespeichert wurden!</font><br /><br /><b>Wollen Sie wirklich eine neue Popup-Programmfunktion ausführen, Änderungen durchführen<br />und anschließend die jetzige Programmfunktion automatisch erneut laden?</b>';
			else
				txt='Popup Program Function: Unsaved Data - Really Execute?|<font class="red">Data has been inputed or changed that has not been saved yet!</font><br /><br /><b>Do you really want to execute a new popup program function, change values<br />and reload the present program function automatically again afterwards?</b>';
		break;

		case 13: // Delete Filter?
			if(oFXP.lang == 1)
				txt='Filter löschen?|Wollen Sie diesen Filter wirklich löschen?';
			else
				txt='Delete Filter?|Do you really want to delete this filter?';
		break;
	}

	// Get headline?
	var pp=txt.indexOf('|');
	if(pp > 0)
	{
		if(headline)
			txt=txt.substr(0,pp);
		else
			txt=txt.substr(pp+1);
	}

	return txt;
}

function fxf_fn_getHelpMessage(msg, pt)
{
	var hlp='';
	switch(msg)
	{
		case 1:	// Click on project - Move project
			if(oFXP.lang == 1)
			{
				if(pArray[pt].type == 'T')
 					hlp='Diese Aufgabe verschieben und alle uebergeordneten Teilprojekte anpassen';
 				else
 					hlp='Dieses und alle untergeordneten Teilprojekte/Aufgaben verschieben und alle uebergeordneten Teilprojekte anpassen';
			}
			else
			{
				if(pArray[pt].type == 'T')
 					hlp='Move this task and adjust all superior subprojects';
 				else
 					hlp='Move this and all inferior subprojects/tasks and adjust all superior subprojects';
			}
		break;

		case 2:	// Click on start or end node - Adjust project duration
			if(oFXP.lang == 1)
			{
				if(pArray[pt].type == 'T')
 					hlp='Diese Aufgabe vergroessern/verkleinern und alle uebergeordneten Teilprojekte anpassen';
 				else
 					hlp='Dieses Teilprojekt vergroessern/verkleinern und alle ueber- und untergeordneten Teilrojekte/Aufgaben anpassen';
			}
			else
			{
				if(pArray[pt].type == 'T')
 					hlp='Enlarge/shrink this task and adjust all superior subprojects';
 				else
 					hlp='Enlarge/shrink this subproject and adjust all superior and inferior subprojects/tasks';
			}
		break;
	}

	return hlp;
}

function fxf_fn_getKey(k)
{
	if(oFXP.lang == 1)
	{
		if(k == 'Shift')
			k='Umschalt';
		else if(k == 'Ctrl')
			k='Strg';
		else if(k == 'Del')
			k='Entf';
		else if(k == '<')
			k='&lt;';
	}

	return '['+k+']';
}

function fxf_fn_question(h,t,b,f,bw)
{
	// Text Popup?
	if(oID.txtpoptoggle.style.display != 'none')
		oID.txtpoptoggle.style.display='none';

	// Headline
	if(h.length)
		oID.iatitle.innerHTML=h;
	else if(oFXP.lang == 1)
		oID.iatitle.innerHTML='Frage/Hinweis';
	else
		oID.iatitle.innerHTML='Question/Note';
	// Text
	var ih='<table width="100%" height="100%" border="0" cellpadding="0" cellspacing="0">';
	ih += '<tr><td align="center" height="100%" style="background:#ffffff;box-shadow:rgba(0,0,0,0.15) 1px 1px inset;padding:8px;">'+t+'</td></tr>';
	ih += '<tr><td align="center" valign="middle"><br />';
	// Buttons
	var sbw='';
	if(bw > 0)
		sbw='widtgh:'+bw+'px;';
	for(var bc=0; bc<b.length; bc++)
		ih += '<input id="btn_q'+bc+'" class="btn" type="submit" value="'+b[bc]+'" style="'+sbw+'margin:0 8px;">';
	ih += '<br /><br /></td></tr>';
	ih += '</table>';
	oID.iaentries.innerHTML=ih;

	aEl=document.activeElement;

	// Body dimensions and scrolls offsets
	var qw=Math.min(760, dim.sd.pwidth-64);
	var qh=Math.min(340, dim.sd.pheight-64);

	oID.iacont.style.width=(qw+20)+'px';
	oID.iatitle.style.width=qw+'px';
	oID.iabox.style.height=qh+'px';
	oID.iaentries.style.width=(qw-20)+'px';
	oID.iaentries.style.height=(qh-24)+'px';

	oID.iacont.style.top=Math.floor(dim.sd.pheight/2 - qh/2)+'px';
	oID.iacont.style.left=Math.floor(dim.sd.pwidth/2 - qw/2)+'px';

	fstack=f;
	oID.iact.style.display='';
	oID.iacont.style.display='';
}

function fxf_fn_drawMenu(id, mtr)
{
	if(mtr === undefined)
		mtr=oFXP.tr;
	else
		mtr=parseInt(mtr);
//alert('fxf_fn_drawMenu(id='+id+', mtr='+mtr+')');
	if(oFXP.mstructure.length && (id != '--'))
	{
		var m1id='';
		var m2id='';
		var ml=0;
		if(id.length)
		{
			if(oFXP.mnid.length)
			{
				var mn1id='';
				var mn2id='';
				var mnl=0;
				var mns=oFXP.mnid.split('_');
				mnl=mns.length;
				mn1id=mns[0];
				if(mnl > 1)
					mn2id=mn1id+'_'+mns[1];
				if((id == mn1id) || (id == mn2id))
					id=id.substr(0,id.length-3);
			}
			var ms=id.split('_');
			ml=ms.length;
			m1id=ms[0];
			if(ml > 1)
				m2id=m1id+'_'+ms[1];
//alert('oFXP.mnid='+oFXP.mnid+' - id='+id);
		}
		oFXP.mnid=id;
//alert('id='+id+', ml='+ml+' -- m1id='+m1id+', m2id='+m2id);

		// Level 1 (always)
		var txt='';
		for(var i=0; i<oFXP.mstructure.length; i++)
		{
			if(oFXP.mstructure[i].lvl == 0)
			{
				if(txt.length)
					txt += '<span class="menu-sl menu1-sl"></span>';
				txt += fxf_fn_drawMenuEntry(oFXP.mstructure[i],m1id,bid);
			}
		}
		if(txt.length)
		{
			oID.fxmenu1c.innerHTML=txt;
			oID.fxmenu1l.style.display='none';
			oID.fxmenu1r.style.display='none';
			oID.fxmenu1.style.display='';
			if(!ml)
				oID.fxmenu1c.style.left=0;
			if(!m1id.length && !m2id.length)
				fxf_fn_drawMenuNav(0);
		}
		else
			oID.fxmenu1.style.display='none';

		// Border
		var bid=fxf_fn_drawMenuBorder(id, mtr);

		// Level 2
		if(ml > 0)
		{
			oID.fxmenu2f.className='menu2-'+m1id+'i';
			oID.fxmenu2l.className='menu menu-h menu2-'+m1id+'i';
			oID.fxmenu2r.className='menu menu-h menu2-'+m1id+'i';
			txt='';
			for(var i=0; i<oFXP.mstructure.length; i++)
			{
				if((oFXP.mstructure[i].lvl == 1) && (oFXP.mstructure[i].id.substr(0,2) == m1id))
				{
					if(txt.length)
						txt += '<span class="menu-sl menu2-'+m1id+'-sl"></span>';
					txt += fxf_fn_drawMenuEntry(oFXP.mstructure[i],m2id,bid);
				}
			}
			if(txt.length)
			{
				oID.fxmenu2c.innerHTML=txt;
				oID.fxmenu2l.style.display='none';
				oID.fxmenu2r.style.display='none';
				if(oID.fxmenu2.style.display == 'none')
				{
					oID.fxmenu2.setOpacity(0);
					oID.fxmenu2.style.top='18px';
					oID.fxmenu2.style.display='';
					oID.fxmenu2c.style.left=0;
					new Effect.Morph(oID.fxmenu2, {style:'top:48px;', duration: 0.25});
					new Effect.Fade(oID.fxmenu2, {from:0.0, to:1.0, duration: 0.25});
					if(m1id.length && !m2id.length)
						fxf_fn_drawMenuNav.delay(0.35, 1);
				}
				else if(m1id.length && !m2id.length)
					fxf_fn_drawMenuNav(1);
			}
			else
				oID.fxmenu2.style.display='none';
		}
		else
			oID.fxmenu2.style.display='none';

		// Level 3
		if(ml > 1)
		{
			oID.fxmenu3f.className='menu3-'+m1id+'i';
			oID.fxmenu3l.className='menu menu-h menu3-'+m1id+'i';
			oID.fxmenu3r.className='menu menu-h menu3-'+m1id+'i';
			txt='';
			for(var i=0; i<oFXP.mstructure.length; i++)
			{
				if((oFXP.mstructure[i].lvl == 2) && (oFXP.mstructure[i].id.substr(0,5) == m2id))
				{
					if(txt.length)
						txt += '<span class="menu-sl menu3-'+m1id+'-sl"></span>';
					txt += fxf_fn_drawMenuEntry(oFXP.mstructure[i],id,bid);
				}
			}
			if(txt.length)
			{
				oID.fxmenu3c.innerHTML=txt;
				oID.fxmenu3l.style.display='none';
				oID.fxmenu3r.style.display='none';
				if(oID.fxmenu3.style.display == 'none')
				{
					oID.fxmenu3.setOpacity(0);
					oID.fxmenu3.style.top='18px';
					oID.fxmenu3.style.display='';
					oID.fxmenu3c.style.left=0;
					new Effect.Morph(oID.fxmenu3, {style:'top:48px;', duration: 0.25});
					new Effect.Fade(oID.fxmenu3, {from:0.0, to:1.0, duration: 0.25});
					if(m1id.length && m2id.length)
						fxf_fn_drawMenuNav.delay(0.35, 2);
				}
				else if(m1id.length && m2id.length)
					fxf_fn_drawMenuNav(2);
			}
			else
				oID.fxmenu3.style.display='none';
		}
		else
			oID.fxmenu3.style.display='none';
	}
	else
	{
		oID.fxmenu1.style.display='none';
		oID.fxmenu2.style.display='none';
		oID.fxmenu3.style.display='none';
		oFXP.mnid='';

		// Border
		var bid=fxf_fn_drawMenuBorder(mtr);
	}
}

function fxf_fn_drawMenuBorder(id, mtr)
{
	if(mtr === undefined)
		mtr=oFXP.tr;
	else
		mtr=parseInt(mtr);
//alert('fxf_fn_drawMenuBorder(id='+id+', mtr='+mtr+')');
	var bid='';
	if(oFXP.mstructure.length)
	{
		// Border
		for(var i=0; i<oFXP.mstructure.length; i++)
		{
			if(mtr == oFXP.mstructure[i].tr)
			{
				bid=oFXP.mstructure[i].id.substr(0,2);
				break;
			}
		}
		var bc='';
		if(bid.length)
		{
			oID.fxmenu1.className='menu1-bt menu1-'+bid+'b';
			bc=oID.fxmenu1.getStyle('border-top-color');
		}
		for(var i=0; i<oFXP.mstructure.length; i++)
		{
			if(oFXP.mstructure[i].lvl == 0)
			{
				var mo=$('fxm_'+oFXP.mstructure[i].id+'_d');
				if(mo)
				{
					if(!id.length && bc.length && (bid == oFXP.mstructure[i].id))
						mo.style.background='-webkit-linear-gradient(top,'+bc+',transparent)';
					else
						mo.style.background='';
				}
			}
		}
	}

	return bid;
}

function fxf_fn_drawMenuEntry(mObj,chkid,bid)
{
	var txt='<span id="fxm_'+mObj.id+'_d" class="menu ';
	var cls='menu'+(mObj.lvl+1)+'-'+mObj.id.substr(0,2);
	if(mObj.id == chkid)
		cls += 'a';
	txt += cls+'"';
	if(mObj.tr)
	{
		var mtr=parseInt(mObj.tr);
		// Handle reports
		if(mtr > 88000)
			mtr=88;
		txt += ' tr="'+mtr+'"';
	}
	if(mObj.arg && mObj.arg.length)
		txt += ' arg="'+mObj.arg+'"';

	var bc='';
	var st='';
	if(bid == mObj.id)
	{
		bc=oID.fxmenu1.getStyle('border-top-color');
		if(bc.length)
			st += 'background:-webkit-linear-gradient(top,'+bc+',transparent);';
	}
	if(mObj.tr)
		st += 'text-shadow:1px 0 #000;';
	if(st.length)
		txt += ' style="'+st+'"';
//txt += ' tooltip="id='+mObj.id+'<br />chkid='+chkid+'<hr />lvl='+mObj.lvl+'<br />tr='+mObj.tr+'<br />arg='+mObj.arg+'<hr />'+cls+'<hr />bid='+bid+' -- bc='+bc+'"';
	if(mObj.tooltip && mObj.tooltip.length)
		txt += ' tooltip="'+mObj.tooltip+'"';
	txt += '>';
	txt += mObj.txt;
	if(!mObj.tr)
		txt += '&nbsp;<span id="fxm_'+mObj.id+'_a" style="font-size:80%;">&#x25BC;</span>';
	txt += '</span>';

	return txt;
}

function fxf_fn_drawMenuNav(ml)
{
	if(!ml)
	{
		oID.fxmenu1l.style.display='none';
		oID.fxmenu1r.style.display='none';
		var m1=fxf_fn_getBCR(oID.fxmenu1c);
		var l=parseInt(oID.fxmenu1c.style.left);
		if(m1.width > dim.sd.pwidth)
		{
			if(l < 0)
				oID.fxmenu1l.style.display='';
			if(l+m1.width > dim.sd.pwidth)
				oID.fxmenu1r.style.display='';
		}
	}
	else if(ml == 1)
	{
		oID.fxmenu2l.style.display='none';
		oID.fxmenu2r.style.display='none';
		var m2=fxf_fn_getBCR(oID.fxmenu2c);
		var l=parseInt(oID.fxmenu2c.style.left);
		if(m2.width > dim.sd.pwidth)
		{
			if(l < 0)
				oID.fxmenu2l.style.display='';
			if(l+m2.width > dim.sd.pwidth)
				oID.fxmenu2r.style.display='';
		}
	}
	else if(ml == 2)
	{
		oID.fxmenu3l.style.display='none';
		oID.fxmenu3r.style.display='none';
		var m3=fxf_fn_getBCR(oID.fxmenu3c);
		var l=parseInt(oID.fxmenu3c.style.left);
		if(m3.width > dim.sd.pwidth)
		{
			if(l < 0)
				oID.fxmenu3l.style.display='';
			if(l+m3.width > dim.sd.pwidth)
				oID.fxmenu3r.style.display='';
		}
	}
}

function fxf_fn_menuNav(ml, d)
{
//alert('Menu Nav: ml='+ml+', d='+d);
	var m=null;
	var l=0;
	if(!ml)
	{
		m=fxf_fn_getBCR(oID.fxmenu1c);
		l=parseInt(oID.fxmenu1c.style.left);
	}
	else if(ml == 1)
	{
		m=fxf_fn_getBCR(oID.fxmenu2c);
		l=parseInt(oID.fxmenu2c.style.left);
	}
	else if(ml == 2)
	{
		m=fxf_fn_getBCR(oID.fxmenu3c);
		l=parseInt(oID.fxmenu3c.style.left);
	}

	if(m)
	{
		if(d == 'r')	// Click on right arrow	-> scroll left
			var nl=Math.max(dim.sd.pwidth-m.width, l-dim.sd.pwidth+96);
		else			// Click on left arrow	-> scroll right
			var nl=Math.min(0, l+dim.sd.pwidth-96);

		if(!ml)
			new Effect.Morph(oID.fxmenu1c, {style:'left:'+nl+'px;', duration: 0.25});
		else if(ml == 1)
			new Effect.Morph(oID.fxmenu2c, {style:'left:'+nl+'px;', duration: 0.25});
		else
			new Effect.Morph(oID.fxmenu3c, {style:'left:'+nl+'px;', duration: 0.25});
		fxf_fn_drawMenuNav.delay(0.30, ml);
	}
}

function fxf_fn_hlpPopup(toggle)
{
	var disp=true;
	if(toggle)
	{
		if(oID.iact.style.display != 'none')
			disp=false;
	}

	if(disp)
	{
		var ll=oFXP.lang;
		var llang=$('log_lang');
		if(llang)
			ll=parseInt(llang.innerHTML);
		var tfa='';
		if($('qrcode_l'))
			tfa='&tfa=1';
		var href=fxf_fn_gProgram('popup_hlp', 'htr='+oFXP.tr+tfa+'&lang='+ll+fxf_fn_gParam());
		imode='hlp';
		fxf_fn_fxLink(false,href);
	}
	else
		fxf_fn_fxLinkClose();
}

function fxf_fn_setPopup(toggle)
{
	var disp=true;
	if(toggle)
	{
		if(oID.iact.style.display != 'none')
			disp=false;
	}

	if(disp)
	{
		var href=fxf_fn_gProgram('popup_setting', 'filter='+gSet.filter+'&help='+gSet.help+'&trmark='+gSet.trmark+fxf_fn_gParam());
		if((oFXP.tr == 23) || (oFXP.tr == 189))
			href += '&ppsppopalways='+gSet.ppsppopalways;
		if(oFXP.tr == 189)
		{
			href += '&alignworkday='+gSet.alignworkday+'&showframes='+gSet.showframes+'&showlines='+gSet.showlines+'&showhr='+gSet.showhr+'&contnumbers='+gSet.contnumbers;
			if(hrheight)
				href += '&wlscbl='+tSet.wlscbl+'&wlscba='+tSet.wlscba+'&wlscbp='+tSet.wlscbp+'&wlscbs='+tSet.wlscbs+'&wlsras='+tSet.wlsras;
		}
		imode='set';
		fxf_fn_fxLink(false,href);
	}
	else
		fxf_fn_fxLinkClose();
}

function fxf_fn_favPopup(sm)
{
	if(sm)
	{
		var arr=oID.fxfav.cumulativeOffset();
		var ri=dim.sd.pwidth-arr[0]-oID.fxfav.clientWidth-2;
//alert('x: arr[0]='+arr[0]+', y: arr[1]='+arr[1]+', w='+oID.fxfav.clientWidth+', h='+oID.fxfav.clientHeight+' -- dim.sd.pwidth='+dim.sd.pwidth+', dim.sd.pheight='+dim.sd.pheight+' -- right='+ri);

		oID.favpop.innerHTML='&nbsp;<font class="darkgrey">loading...</font>';
		oID.favpop.style.right=ri+'px';
		oID.favpop.style.display='';

		var url=fxf_fn_worker('favorites','');
		new Ajax.Request(url,
		{
			method:'get', asynchronous:true,
			onSuccess: function(transport)
			{
//fxf_fn_writeDebug('log', transport.responseText);
				oID.favpop.innerHTML=transport.responseText;
			},
			onFailure: function()
			{
				oID.favpop.style.display='none';
			}
		});
	}
	else
		oID.favpop.style.display='none';
}

function fxf_fn_searchPopup(sm)
{
	if(sm)
	{
		oID.fxsrcinput.style.width=tblwidth+'px';
		oID.fxfltsel.style.width='0';
		var sv=oID.fxsrcinput.value;
		if((sv == 'Suche') || (sv == 'Search'))
			oID.fxsrcinput.value='';
		oID.fxsrcinput.style.color='#000000';
		var url=fxf_fn_worker('search','search='+oID.fxsrcinput.value);
//alert('searchPopup: url='+url);
		if(oID.searchpop.style.display == 'none')
		{
			oID.searchpop.innerHTML='&nbsp;<font class="darkgrey">loading...</font>';
			oID.searchpop.style.display='';
		}
		new Ajax.Request(url,
		{
			method:'get', asynchronous:true,
			onSuccess: function(transport)
			{
//fxf_fn_writeDebug('log', transport.responseText);
				oID.searchpop.innerHTML=transport.responseText;
			},
			onFailure: function()
			{
				oID.searchpop.style.display='none';
			}
		});
	}
	else
	{
		oID.fxsrcinput.style.width=tbswidth+'px';
		oID.fxfltsel.style.width=tbswidth+'px';
		oID.searchpop.style.display='none';
		var sv=oID.fxsrcinput.value;
		if(!sv.length || (sv == 'Suche') || (sv == 'Search') || (sv == '*screenshot'))
		{
			if(oFXP.lang == 1)
				oID.fxsrcinput.value='Suche';
			else
				oID.fxsrcinput.value='Search';
			oID.fxsrcinput.style.color='#a4a4a4';

			if(sv == '*screenshot')
			{
				var fxsm=$('fxscrshtmd');
				if(fxsm)
				{
					var disp='none';
					if(fxsm.style.display == 'none')
						var disp='';
					fxsm.style.display=disp;
					if(disp == 'none')
					{
						oID.fxworkf.style.background='#f0f0f0';
						oID.fxwork.style.background='#f0f0f0';
						var pels=$$('[id^="fxphoto_"]');
						if(pels.length)
						{
							for(var pc=0; pc<pels.length; pc++)
								pels[pc].style.display='none';
						}
					}
				}
			}
		}
		else
			oID.fxsrcinput.style.color='#000000';
	}
}

function fxf_fn_tooltipPopup(event)
{
	// Get element with attribute "tooltip"
	var element=fxf_fn_getField($(Event.element(event)),false);
	while(element && (!element.attributes || !element.attributes['tooltip']))
	{
		element=fxf_fn_getField(element.up(),false);
	}
	if(!element || !element.attributes || !element.attributes['tooltip'])
		return;
	var tv=element.attributes['tooltip'].value;
	if(!tv.length)
		return;

	// Extract headline and data
	var th='';
	var td='';

	var tl=tv.toLowerCase();
	var tp=tl.indexOf('<table');
	if(tp >= 0)
	{
		th=tv.substr(0,tp).replace(/\<div *.+?\>/gi, '').replace(/\<\/div *?\>/gi, '').replace(/\<img +.+?\>/i, '').replace(/\<br *\/*\>/gi, '');
		td=tv.substr(tp).replace(/display: *none *;*/gi, 'display:table-row;').replace(/\<tr +note +.+?\<\/tr *\>/i, '').replace(/\<br *\/*\>$/gi, '');	// Display all table rows, but delete the row with attribute "note"
	}
	else
		td=tv.replace(/display: *none *;*/gi, 'display:;');
//alert('MaxDimensions: width='+dim.wd.width+', height='+dim.wd.height+'\n\nth:\n'+th+'\n\ntd:\n'+td);

	// Wrap everything into a scrollable div
	var ntd='<div style="width:auto;max-width:'+dim.wd.width+'px;height:auto;max-height:'+dim.wd.height+'px;overflow:auto;">';
	ntd += td;
	ntd += '</div>';

	// Display the popup
	fxf_fn_fxLinkDisplay(ntd, th);
}

function fxf_fn_fxLinkClose()
{
	oID.iact.style.display='none';
	oID.iacont.style.display='none';
	oID.iainfo.innerHTML='<input id="iainfofocus" type="text" value="" style="border:0;background:transparent;position:absolute;left:0;top:-'+dim.sd.pheight+'px;width:0;height:0;">';
	oID.iainfo.style.display='none';
	imode='';
	oID.hrppsppop.style.display='none';
	if(dateElem)
	{
		dateElem.parentNode.removeChild(dateElem);
		dateElem=null;
	}
	if(aEl)
	{
		aEl.focus();
		aEl=null;
	}

	fxf_fn_prjcatClose();
	oID.oUploader.uploader.style.display='none';
}

function fxf_fn_fxLinkDisplay(data, title, print)
{
	if(oID.txtpop.style.display != 'none')
		return;

	if(!imode.length)
		imode='inf';

	if(title == undefined)
		title='';

	if((print == undefined) || print)
		print=true;
	else
		print=false;

	if(title.length)
	{
		if(print)
		{
			var ptt='Print';
			if(oFXP.lang == 1)
				ptt='Drucken';
		}

		var ctt='Close';
		if(oFXP.lang == 1)
			ctt='Schließen';
		ctt += ' <i class=\'s2 lightergrey\'>(Esc)</i>';

		var d='<div class="popup" style="position:relative;padding:6px;white-space:normal;width:fit-content;">';
		d += '<br />';
		d += '	<div style="position:absolute;left:0;top:0;width:100%;height:auto;">';
		d += '		<table width="100%" border="0" align="left" cellspacing="0" cellpadding="1">';
		d += '			<tr>';
		d += '				<td class="pmtitle" width="100%" valign="top" onmousedown="fxf_fn_dragIt(event, \'iainfo\');">';
		d += '					'+title+'&nbsp;&nbsp;';
		d += '					<img id="pop_ic_close" class="sic_close" src="./GFX/ic_cls_b_36x24.png" tooltip="'+ctt+'" onclick="fxf_fn_fxLinkClose();" onmouseover="$(this).src=\'./GFX/ic_cls_w_36x24.png\';" onmouseout="$(this).src=\'./GFX/ic_cls_b_36x24.png\';" style="position:absolute;right:0;top:0;width:36px;height:24px;border-top-right-radius:5px;cursor:pointer;">';
		if(print)
			d += '					<img id="pop_ic_print" class="sic" src="./GFX/ic_prt_b_36x24.png" tooltip="'+ptt+'" onclick="fxf_fn_print(\'fxp_prt_div\', \''+title+'\');" style="position:absolute;right:36px;top:0;cursor:pointer;">';
		d += '				</td>';
		d += '			</tr>';
		d += '		</table>';
		d += '	</div><br />';
		d += '	<div id="fxp_prt_div">'+data+'</div>';
		d += '</div>';

		data=d;
	}

	oID.iainfo.style.width='auto';
	oID.iainfo.style.height='auto';
	oID.iainfo.innerHTML='<input id="iainfofocus" type="text" value="" style="border:0;background:transparent;position:absolute;left:0;top:-'+dim.sd.pheight+'px;width:0;height:0;">'+data;

	aEl=document.activeElement;

	fxf_fn_shrinkWorkflow(0.2,-1);
	fxf_fn_shrinkProjects(0.2,-1);

	fxf_fn_waitScreen('');
	oID.iact.style.display='';
	oID.iainfo.style.display='';
	$('iainfofocus').focus();

	var ieh=oID.iainfo.clientHeight+24;
	var iew=oID.iainfo.clientWidth+24;
//alert('ieh='+ieh+', iew='+iew);

	// Scrolls offsets
	var qw=Math.min(iew, dim.sd.pwidth-64);
	var qh=Math.min(ieh, dim.sd.pheight-64);
//alert('qw='+qw+', qh='+qh);

	oID.iainfo.style.width=qw+'px';
	oID.iainfo.style.height=qh+'px';
	oID.iainfo.style.top=Math.floor(dim.sd.pheight/2 - qh/2)+'px';
	oID.iainfo.style.left=Math.floor(dim.sd.pwidth/2 - qw/2)+'px';
}

function fxf_fn_vpGetWidth()
{
	var vpw=parseInt(self['innerWidth'] || document.documentElement['clientWidth'] || document.body['clientWidth']);
	return vpw;
}

function fxf_fn_vpGetHeight()
{
	var vph=parseInt(self['innerHeight'] || document.documentElement['clientHeight'] || document.body['clientHeight']);
	return vph;
}

function fxf_fn_highlightLine(trelem, highlight)
{
	// Find first td element
	var tdelem=trelem.down();
	// Highlight all td
	while(tdelem)
	{
		if(highlight)
			tdelem.style.backgroundColor='#eeeeff';
		else
			tdelem.style.backgroundColor='';

		tdelem=tdelem.next();
	}
}

function fxf_fn_undoDisabled()
{
	oFXP.field.className=oFXP.field.className.substr(0,oFXP.field.className.length-1);
	oFXP.field.removeAttribute('ud');
	oFXP.field.removeAttribute('tooltip');
	oFXP.field.style.cursor='';
	if(oFXP.field.readOnly)
		oFXP.field.readOnly=false;
	if(oFXP.field.disabled)
		oFXP.field.disabled=false;

	oFXP.field.focus();

	oFXP.field=null;
}

// Prompt before releasing a semi disabled field back for changing
function fxf_fn_askUndoDisable(element)
{
	oFXP.field=element;
	fxf_fn_waitScreen('');
	fxf_fn_question(fxf_fn_getMessage(8,true), fxf_fn_getMessage(8,false), [fxf_fn_getText(10), fxf_fn_getText(11)], ['fxf_fn_undoDisabled();', 'oFXP.field=null;'], 100);
}

// Text Popup
function fxf_fn_textPopup(mode,element,event)
{
	if(mode < 0)
	{
		oID.txtpoptoggle.style.display='none';
		oID.txtpop.style.display='none';
		oVar.lastTxtUID='';
		oVar.lastTxtElement=null;
	}
	else
	{
		oVar.lastTxtUID=element.id;
		oVar.lastTxtElement=element;
		var ed=fxf_fn_getBCR(element);

		if(!mode)
		{
			oID.txtpop.style.display='none';
			oID.txtpoptoggle.style.left=(ed.right-1)+'px';
			oID.txtpoptoggle.style.top=(ed.top-1)+'px';
			if(element.className.substr(element.className.length-1,1) == 'm')
				oID.txtpoptoggle.style.borderColor='#79aec7';
			else
				oID.txtpoptoggle.style.borderColor='#c0c0c0';
			oID.txtpoptoggle.style.display='';
		}
		else
		{
			oID.txtpoptoggle.style.display='none';
			aEl=element;
			var maxlength=0;
			if(element.readAttribute('maxlength'))
				maxlength=parseInt(element.readAttribute('maxlength'));
			var pw=Math.min(dim.wd.width-52, Math.max(960,ed.width-6));
			var ph=Math.min(dim.wd.height-12, Math.max(320,ed.height-10));
			var pl=Math.max(dim.wd.left, Math.min(ed.left, dim.wd.right-52-pw));
			var pt=Math.max(dim.wd.top, Math.min(ed.top, dim.wd.bottom-12-ph));
			oID.txtpop.style.width=pw+'px';
			oID.txtpop.style.height=ph+'px';
			oID.txtpop.style.left=pl+'px';
			oID.txtpop.style.top=pt+'px';
			if(element.className.substr(element.className.length-1,1) == 'm')
				oID.txtpopta.className='dfxftam sh44320m';
			else
				oID.txtpopta.className='dfxfta sh44320';
			oID.txtpopta.value=element.value;
			oID.txtpop.style.display='';
			oID.txtpopta.focus();
//fxf_fn_taskMsg('ed: '+ed.left+','+ed.top+','+ed.right+','+ed.bottom+' ('+ed.width+'x'+ed.height+') -- wd: '+dim.wd.left+','+dim.wd.top+','+dim.wd.right+','+dim.wd.bottom+' ('+dim.wd.width+'x'+dim.wd.height+') -> pd: '+pl+','+pt+' ('+pw+'x'+ph+')');
		}
	}
}

function fxf_fn_textSet(element)
{
	if(element.type != 'textarea')
		oID.txtpopta.value=oID.txtpopta.value.replace(/%0D%0A/gi, ' ').replace(/%0A/gi, ' ');
	if(element.readAttribute('maxlength'))
	{
		maxlength=parseInt(element.readAttribute('maxlength'));
		if(maxlength > 0)
			oID.txtpopta.value=oID.txtpopta.value.substr(0,maxlength);
	}
	element.value=oID.txtpopta.value;

	if(oFXP.tr != 110)
	{
		element=fxf_fn_getField(element,true);
		fxf_fn_saveElement(element);
	}

	fxf_fn_textPopup(-1,element);

	if(oFXP.tr == 110)
	{
		oFXP.addchg=true;
		fxf_fn_dmsSave('change', element.id);
	}
}

// Checkbox Popup
function fxf_fn_checkboxPopup(element, event)
{
	var href=fxf_fn_gProgram('popup_chk', 'mnr='+element.id.substr(5)+fxf_fn_gParam());
//alert('checkboxPopup: href='+href);
	fxf_fn_fxLink(element,href);
}

function fxf_fn_checkboxSet(set,mnr,fieldname)
{
//alert('fxf_fn_checkboxSet('+set+','+mnr+','+fieldname+')');
	// Special handling for 56: "Add Programs to Profile"
	if(oFXP.tr == 56)
	{
		fxf_fn_cbClick(0,fieldname.substr(4),-1,-1,set);
		return;
	}

	// Get all checkbox elements
	var cels = $$('[ref="fx_cb_'+mnr+'"]');
//alert('cels='+cels);
	if(cels && cels.length)
	{
		var fl=fieldname.length;
		var cchanged='';
		var frm='';
		for(var c=0; c<cels.length; c++)
		{
			var cn=cels[c].name.substr(0,fl);
//alert(c+': id='+cels[c].id+', name='+cels[c].name+', disabled='+cels[c].disabled+' -- '+fieldname+' = '+cn);
			if(!cels[c].disabled && (!fl || (fieldname == cn)))
			{
				var cc=false;
				if(cels[c].checked)
					cc=true;

				if(set)
					cels[c].checked=true;
				else
					cels[c].checked=false;

				// Save changes
				if((set && !cc) || (!set && cc))
				{
					if(cchanged.length)
						cchanged += '|';
					if(set)
						cchanged += '1';
					else
						cchanged += '0';
					cchanged += cels[c].name;

					if(!frm.length && cels[c].attributes && (typeof cels[c].attributes['fxform'] != 'undefined'))
						frm=cels[c].attributes['fxform'].value;
				}
			}
		}

		if(cchanged.length)
		{
fxf_fn_writeDebug('info', 'checkboxSet: frm='+frm+', cchanged.length='+cchanged.length);
			var url=fxf_fn_worker('setcb','keep_post=1&frm='+frm);
			var pst='cbs='+cchanged;
//alert('url='+url+'\npst='+pst);
			new Ajax.Request(url,
			{
				method:'post', postBody:pst, asynchronous:true,
				onSuccess: function(transport)
				{
fxf_fn_writeDebug('log', transport.responseText);
				}
			});
		}
	}
}

// Datums-Popup
function fxf_fn_openDatePopUp(date_field, event)
{
	if(!date_field)
		return;

	if(dateElem)
	{
//alert('Element exists already, remove...');
		dateElem.parentNode.removeChild(fid);
		dateElem=null;
		oID.iact.style.display='none';
		return;
	}

	var fleft=Math.max(0, Event.pointerX(event)-122);
	var ftop=Math.max(0, Event.pointerY(event)-88);
//alert('Create date popup: field='+date_field.name+', fleft='+fleft+', ftop='+ftop);

	oID.iact.style.display='';

	var cont=new Element('div', {id:'cal_'+date_field.name, style:'position:absolute;left:'+fleft+'px;top:'+ftop+'px;border:0;background-color:transparent;'});
	$(document.body).insert(cont);

	fxf_fn_ajaxDate(date_field.name, '');
}

function fxf_fn_ajaxDate(fieldname, fielddate)
{
	if(!fieldname || (fieldname.length == 0))
		return;

	var textfieldid=$(fieldname);
	dateElem=$('cal_'+fieldname);
//alert('fieldname='+fieldname+', fielddate='+fielddate+'\ntextfieldid='+textfieldid+', dateElem='+dateElem);
	if(!textfieldid || !dateElem)
		return;

	var mark='';
	var timestamp=fxf_fn_convertDate2Timestamp(textfieldid.value).timestamp;
	if(textfieldid.value.length)
		mark=timestamp;
	else if(fieldname == 'Soll_Beg_Dtm')
	{
		var sde=$('Soll_End_Dtm');
		if(sde && sde.value && sde.value.length)
		{
			timestamp=fxf_fn_convertDate2Timestamp(sde.value).timestamp;
			mark=timestamp;
		}
	}
	else if(fieldname == 'Soll_End_Dtm')
	{
		var sde=$('Soll_Beg_Dtm');
		if(sde && sde.value && sde.value.length)
		{
			timestamp=fxf_fn_convertDate2Timestamp(sde.value).timestamp;
			mark=timestamp;
		}
	}

	if(!fielddate || (fielddate.length == 0))
		fielddate=timestamp;
	else if(fielddate == 'close')
	{
		dateElem.parentNode.removeChild(dateElem);
		dateElem=null;
		oID.iact.style.display='none';
		return;
	}

	var person=0;
	var personid=$('Personen_ID');
	if(personid)
	{
		var persobj=fxf_fn_getSelectedValue(personid);
		if(persobj)
			person=parseInt(persobj.value);
	}
	if(!person)
	{
		var calpopperson=$('cal_pop_person');
		if(calpopperson)
			person=parseInt(calpopperson.innerHTML);
	}

	var url=fxf_fn_gProgram('popup_cal', 'fieldname='+fieldname+'&fielddate='+fielddate+'&mark='+mark+'&person='+person+fxf_fn_gParam());
//alert('url='+url);
	new Ajax.Request(url,
	{
		method: 'get',

		onSuccess: function(transport)
		{
//alert('transport.responseText=\n'+transport.responseText);
			dateElem.innerHTML=transport.responseText;
		},

		onFailure: function(failure)
		{
			dateElem.parentNode.removeChild(dateElem);
			dateElem=null;
			oID.iact.style.display='none';
		}
	});
}

function fxf_fn_changeDay(fieldname, year, month, day)
{
	if(!fieldname || (fieldname.length == 0))
	{
//alert('fxf_fn_changeDay('+year+', '+month+', '+day+')');
		fxf_fn_waitScreen('loading');
		fxf_fn_loadTR(oFXP.tr,'datenav&get=1&gd='+fxf_fn_getTimestamp(year, month, day));
	}

	var textfieldid=fxf_fn_getField($(fieldname),false);
	if(!textfieldid || !dateElem)
		return;

	textfieldid.value=fxf_fn_convertTimestamp2Date(fxf_fn_getTimestamp(year, month, day)).date;
	textfieldid.fxv=textfieldid.value;

	dateElem.parentNode.removeChild(dateElem);
	dateElem=null;
	oID.iact.style.display='none';
	fxf_fn_saveElement(textfieldid);
}

function fxf_fn_changeMonth(fieldname, year, month, day)
{
	var monthid=fxf_fn_getElement('cal_month_'+fieldname);
	if(!monthid)
		return;

	if(!month)
		month=parseInt(monthid.fxv);

	if(!fieldname || (fieldname.length == 0))
	{
//alert('fxf_fn_changeMonth('+year+', '+month+', '+day+')');
		fxf_fn_waitScreen('loading');
		fxf_fn_loadTR(oFXP.tr,'datenav&get=1&gm='+fxf_fn_addLeadingZeros(year,4)+fxf_fn_addLeadingZeros(month,2)+'00000000');
	}

	fxf_fn_ajaxDate(fieldname, fxf_fn_getTimestamp(year, month, day));
}

function fxf_fn_changeYear(fieldname, year, month, day)
{
	var yearid=fxf_fn_getElement('cal_year_'+fieldname);
	if(!yearid)
		return;

	var minmax_yearid=$('cal_minmax_year_'+fieldname);
	if(!minmax_yearid)
		return;

	var minmax_arr=minmax_yearid.innerHTML.split(',');
	var minyear=parseInt(minmax_arr[0]);
	var maxyear=parseInt(minmax_arr[1]);

	if(!year)
		year=parseInt(yearid.fxv);
	year=Math.max(minyear, Math.min(year, maxyear));

	if(!fieldname || (fieldname.length == 0))
	{
		fxf_fn_waitScreen('loading');
		fxf_fn_loadTR(oFXP.tr,'datenav&get=1&gj='+fxf_fn_addLeadingZeros(year,4));
	}

	fxf_fn_ajaxDate(fieldname, fxf_fn_getTimestamp(year, month, day));
}

function fxf_fn_changeWeek(fieldname, year, month, day, week)
{
//alert('fxf_fn_changeWeek('+year+', '+month+', '+day+', '+week+')');
	fxf_fn_waitScreen('loading');
	fxf_fn_loadTR(oFXP.tr,'datenav&get=1&gw='+fxf_fn_addLeadingZeros(year,4)+fxf_fn_addLeadingZeros(month,2)+'0000'+fxf_fn_addLeadingZeros(day,2)+fxf_fn_addLeadingZeros(week, 2));
}

// Check if element is disabled
function fxf_fn_checkDisabled(elm)
{
	if(elm.disabled)
		return true;
	if(elm.refresh != null)
		elm.refresh();
	return false;
}

function fxf_fn_dragIt(event, id)
{
fxf_fn_writeDebug('log+', '<b class="blue">fxf_fn_dragIt</b>');
	var el;

	Event.stopObserving(document, 'mousemove', fxf_eh_mouseMove, false);

	drag_object.elNode = $(id);

	// Save starting positions of cursor and element.
	drag_object.cursorStartX	= mcx;
	drag_object.cursorStartY	= mcy;
	drag_object.elStartLeft		= parseInt(drag_object.elNode.style.left, 10);
	drag_object.elStartTop		= parseInt(drag_object.elNode.style.top, 10);
	drag_object.elStartWidth	= parseInt(drag_object.elNode.style.width, 10);
	drag_object.elStartHeight	= parseInt(drag_object.elNode.style.height, 10);

	if(id == 'hrpool')
	{
		var hrptc=$('hrptc');
		if(hrptc)
			drag_object.elStartHeight = parseInt(hrptc.style.height, 10);
	}

	if(isNaN(drag_object.elStartLeft))
		drag_object.elStartLeft=0;
	if(isNaN(drag_object.elStartTop))
		drag_object.elStartTop=0;
	if(isNaN(drag_object.elStartWidth))
		drag_object.elStartWidth=0;
	if(isNaN(drag_object.elStartHeight))
		drag_object.elStartHeight=0;

	// Capture mousemove and mouseup events on the page.
	if(Prototype.Browser.IE && (Prototype.BrowserFeatures['Version'] < 11))
	{
		document.attachEvent("onmousemove", fxf_dh_dragMove);
		document.attachEvent("onmouseup", fxf_dh_dragStop);
		window.event.cancelBubble = true;
		window.event.returnValue = false;
	}
	else
	{
		document.addEventListener("mousemove", fxf_dh_dragMove, true);
		document.addEventListener("mouseup", fxf_dh_dragStop, true);
		event.preventDefault();
	}
}

function fxf_dh_dragMove(event)
{
	fxf_eh_mouseMove(event);

	var eid=drag_object.elNode.id;
	if(eid == 'hrpool')
	{
		var hrptc=$('hrptc');
		var rptdd=fxf_fn_getBCR($('rptd'));
		var sdgdd=fxf_fn_getBCR(oID.sdg_container);

		var xd=mcx-drag_object.cursorStartX;
		var yd=mcy-drag_object.cursorStartY;

		var mh=sdgdd.height-115;
		var ml=sdgdd.width-rptdd.width-35;

		drag_object.elNode.style.left=Math.min(ml, Math.max(8,drag_object.elStartLeft+xd))+"px";
		hrptc.style.height=Math.max(20,Math.min(drag_object.elStartHeight-yd,mh))+'px';
//fxf_fn_taskMsg('eid='+eid+': elStartLeft='+drag_object.elStartLeft+', elStartTop='+drag_object.elStartTop+', elStartWidth='+drag_object.elStartWidth+', elStartHeight='+drag_object.elStartHeight+' -- mh='+mh+', ml='+ml+' -- left='+drag_object.elNode.style.left+', height='+hrptc.style.height);
	}
	else
	{
		drag_object.elNode.style.left = (drag_object.elStartLeft-drag_object.cursorStartX+mcx)+"px";
		drag_object.elNode.style.top  = (drag_object.elStartTop-drag_object.cursorStartY+mcy)+"px";
	}

	if(Prototype.Browser.IE)
	{
		window.event.cancelBubble = true;
		window.event.returnValue = false;
	}
	else
		event.preventDefault();
}

function fxf_dh_dragStop(event)
{
	// Stop capturing mousemove and mouseup events.
	if(Prototype.Browser.IE && (Prototype.BrowserFeatures['Version'] < 11))
	{
		document.detachEvent("onmousemove", fxf_dh_dragMove);
		document.detachEvent("onmouseup", fxf_dh_dragStop);
	}
	else
	{
		document.removeEventListener("mousemove", fxf_dh_dragMove, true);
		document.removeEventListener("mouseup", fxf_dh_dragStop, true);
	}

	Event.observe(document, 'mousemove', fxf_eh_mouseMove, false);
}

function fxf_fn_waitScreen(text)
{
	var fwcms=Date.now();
	var fwmd=0;
	if(fwms > 0)
		fwmd=fwcms-fwms;

	// Text Popup?
	if(oID.txtpoptoggle.style.display != 'none')
		oID.txtpoptoggle.style.display='none';
	if(oID.txtpop.style.display != 'none')
		oID.txtpop.style.display='none';

	if(text.length)
	{
		var ht='';
		var dpm=false;
		var ani=true;
		if((text == 'Installing') || (text == 'Updating') || (text == 'Patching'))
			dpm=true;
		else if(text.substr(0,1) == '+')
		{
			dpm=true;
			text=text.substr(1);
		}
		else if(text.substr(0,1) == '-')
		{
			ani=false;
			text=text.substr(1);
		}
		if(text.length > 3)
			ht += '<div style="position:relative;font-size:48px;color:rgba(255,255,255, 0.9);text-shadow:rgba(0,0,0, 0.9) 1px 1px 3px;white-space:nowrap;">'+text+'...</div><br /><br />';

		if(dpm)
			ht += fxf_fn_drawProgressMask(1, text);
		else if(ani)
			ht += '<div class="wacircle1"></div><div class="wacircle2"></div>';

		oID.waitcontent.innerHTML=ht;
 		oID.waitscreen.style.display='';
		achg=true;
		fwms=fwcms;
//alert('fxf_fn_waitScreen(text='+text+')');

		if((text == 'Installing') || (text == 'Updating') || (text == 'Patching'))
			fxf_fn_checkUpdateProgress(0,0);
 	}
	else
 	{
 		oID.waitscreen.style.display='none';
		oID.waitcontent.innerHTML='';
		achg=false;
		fwms=0;
 	}
}

function fxf_fn_drawProgressMask(mode, text)
{
	ht  = '<div style="position:relative;margin:0 auto;width:800px;padding:0;text-align:left;">';
	ht += '	<div id="progress_bar_container">';
	ht += '		<div id="progress_bar" style="display:none;width:0%;"></div>';
	ht += '		<div id="progress_percent" style="font-size:0.8em;font-weight:bold;position:absolute;top:0;right:373px;line-height:20px;padding-right:12px;color:#ffffff;text-shadow:#000000 0 1px 0px;white-space:nowrap;">0%</div>';
	ht += '	</div>';
	ht += '	<div id="progress_glow_container" style="display:none;"><img id="progress_glow" style="display:none;" src="./GFX/glow048.png"></div>';
	if(mode == 2)	// Upload
		ht += ' <div style="position:absolute;top:-3px;right:-128px;"><input class="fxfbs" type="submit" value="Cancel" onclick="fxf_fn_checkUploadProgress(\''+text+'\', -1);"></div>';
	ht += '</div>';
	if(mode == 2)
		ht += '<div style="position:relative;width:640px;height:20px;margin:0 auto;background:#ffffff;border-bottom-left-radius:24px;border-bottom-right-radius:24px;padding-top:4px;white-space:nowrap;"><table width="100%" border="0" cellpadding="0" cellspacing="0"><tr><td width="100%" align="left">&nbsp;&nbsp;&nbsp;&nbsp;<span id="progress_files" class="blue" style="font-size:10px;"></span></td><td align="right"><span id="progress_message" style="font-size:11px;color:#888888;">Starting&nbsp;&hellip;</span>&nbsp;&nbsp;&nbsp;&nbsp;</td></tr></table></div>';
	else
		ht += '<div id="progress_message" style="position:relative;width:640px;height:20px;margin:0 auto;background:#ffffff;border-bottom-left-radius:24px;border-bottom-right-radius:24px;padding-top:4px;font-size:11px;color:#777777;text-align:center;white-space:nowrap;">'+text+'&nbsp;&hellip;</div>';

	return ht;
}

function fxf_fn_drawProgressBar(up, pp, msg)
{
	var pgv=Math.max(0, Math.min(100, parseInt(pp)));

	var pgb=$('progress_bar');
	var pgp=$('progress_percent');
	var pgc=$('progress_glow_container');
	var pgg=$('progress_glow');
	var pgm=$('progress_message');

	var pgs=$('progress_stylesheet');
	if(pgs)
	{
		var s=pgs.parentNode;
		s.removeChild(pgs);
	}

	if((pgv > 0) && (pgv < 100))
	{
		var rule='@keyframes moveGlow {';
		rule += '0% {left:-48px;opacity:0.0;}';
		var pgv2=parseInt(pgv/2);
		if(pgv2 > 0)
			rule += ' '+pgv2+'% {opacity:'+Math.min(0.55, pgv2/50)+';}';
		rule += ' '+pgv+'% {opacity:0.0;}';
		rule += ' 100% {left:740px;opacity:0.0;}';
		rule += '}';

		var pgs=document.createElement('style');
		pgs.id='progress_stylesheet';
		pgs.innerHTML=rule;
		var style=document.body.appendChild(pgs);
	}

	if(pgb && pgp && pgc && pgg)
	{
		if(pgv <= 0)
		{
			pgb.style.display='none';
			pgc.style.display='none';
			pgg.style.display='none';
		}
		else if(pgv >= 100)
		{
			pgb.style.display='';
			if(up >= 0)
				pgb.style.width=pgv+'%';
			pgc.style.display='none';
			pgg.style.display='none';
		}
		else
		{
			pgb.style.display='';
			if(up >= 0)
				pgb.style.width=pgv+'%';
			pgc.style.display='';
			if(up >= 0)
				pgc.style.width=(32 + 8*pgv)+'px';
			pgg.style.display='';
		}
		if(pgm && msg && msg.length)
			pgm.innerHTML=msg;
		if(up >= 0)
			pgp.innerHTML=pgv+'%';
	}
}

function fxf_fn_checkUpdateProgress(pcnt, pp)
{
	if(pp <= 100)
	{
		var url=fxf_fn_gProgram('check_update', 'pcnt='+pcnt+'&pp='+pp+'&lng='+oFXP.lang+fxf_fn_gParam());
//alert('url='+url);

		new Ajax.Request(url,
		{
			method:'get', asynchronous:asajax,
			onSuccess: function(transport)
			{
				var pr=transport.responseText.split('|');
				pp=parseInt(pr[0]);

				fxf_fn_drawProgressBar(1, pp, pr[1]);

				if((pp >= 0) && (pp < 100))
					fxf_fn_checkUpdateProgress.delay(1.0, pcnt+1, pp);
			}
		});
	}
}

function fxf_fn_uploadFile(fieldname)
{
	if(((typeof fieldname == 'undefined') || !fieldname || !fieldname.length) && (oID.oUploader.uploader.style.display != 'none'))
		fieldname=oID.oUploader.uploaderh_upn.value;
	if((typeof fieldname == 'undefined') || !fieldname || !fieldname.length)
		return;

	var uld=0;
	var utype='';
	if(oID.oUploader.uploader.style.display != 'none')
	{
		uld=1;
		var frm=oID.oUploader.uploader_form;
		var fld=oID.oUploader.uploader_select;
		var field=$(fieldname);
		var fxform='';
		if(field && field.attributes && (typeof field.attributes['fxform'] != 'undefined'))
			fxform=field.attributes['fxform'].value;
		if(field && field.attributes && (typeof field.attributes['utype'] != 'undefined'))
			utype=field.attributes['utype'].value.toLowerCase().substr(0,3);
		var uparam='';
		var fieldu=$(fieldname+'_uparam');
		if(!fieldu)
			fieldu=$('uparam');
		if(fieldu)
			uparam=fieldu.value;
		frm.action=fxf_fn_gProgram('upload', 'frm='+fxform+'&utype='+utype+'&uparam='+uparam+'&umulti='+oID.oUploader.multi+fxf_fn_gParam());
		if(oFXP.tr == 110)
		{
			var seid=tSet.dms.aseid;
			if((fieldname == 'checkinfile') && tSet.dms.files.length)
			{
				for(var l=0; l<tSet.dms.files.length; l++)
				{
					if(tSet.dms.files[l].sel)
					{
						seid=tSet.dms.files[l].id;
						break;
					}
				}
			}
			frm.action += '&spath='+fxf_fn_escapeText(tSet.dms.aspath,false)+'&seid='+seid;
		}
		fxf_fn_fxLinkClose();
	}
	else
	{
		var frm=$(fieldname+'_upload_form');
		var fld=$(fieldname);
		$(fieldname+'_upload_status').innerHTML='<img src="GFX/uploading.gif" style="position:absolute;left:1px;top:2px;">';
	}
	var multi=0;
	if(fld.name.substr(fld.name.length-1,1) == ']')
		multi=1;
//alert('fxf_fn_uploadFile(fieldname='+fieldname+')\nuld='+uld+', utype='+utype+'\nfrm='+frm+'\naction='+frm.action+'\nmulti='+multi+', value='+fld.value);

	if(oFXP.tr == 217)	// Form design
	{
		var i=0;
		while(true)
		{
			id_cfolder = document.getElementsByName('mordner')[i];
			if(id_cfolder)
			{
				if(id_cfolder.checked)
					break;
				i++;
			}
			else
				break;
		}
		if(id_cfolder)
			frm.upl_mordner.value=parseInt(id_cfolder.value);
		else
			frm.upl_mordner.value=oSet.client;

		var id_ctype = fxf_fn_getField($('ktyp'),true);
		frm.upl_ktyp.value=parseInt(fxf_fn_getSelectedValue(id_ctype).value);
	}

	var ht='<div style="position:relative;font-size:48px;color:rgba(255,255,255, 0.9);text-shadow:rgba(0,0,0, 0.9) 1px 1px 3px;white-space:nowrap;">uploading...</div><br /><br />';
	ht += fxf_fn_drawProgressMask(2, fieldname);

	oID.waitcontent.innerHTML=ht;
	oID.waitscreen.style.display='';
	fxf_fn_checkUploadProgress.delay(0.25, uld,fieldname,0);

	if(uld && oID.oUploader.filelist.length)
	{
		var act=frm.action+'&nohtml=1';

		// Create new AJAX request
		var xhr=new XMLHttpRequest();
		xhr.open('POST', act);
		xhr.onreadystatechange=function() {
//alert('readyState='+this.readyState+'\nstatus='+this.status+'\nresponseText='+this.responseText);
			if((this.readyState === 4) && (this.status === 200))
				fxf_fn_uploadFileDone.delay(0.5, fieldname, utype, this.responseText);
			return;
		};

		// Create new form
		var formdata=new FormData();
		// ...add hidden fields
		formdata.append(oID.oUploader.uploaderh_frm.name, oID.oUploader.uploaderh_frm.value);
		formdata.append(oID.oUploader.uploaderh_lts.name, oID.oUploader.uploaderh_lts.value);
		formdata.append(oID.oUploader.uploaderh_upn.name, oID.oUploader.uploaderh_upn.value);
		// ...add files
		if(oID.oUploader.filelist.length == 1)
			formdata.append(fieldname, oID.oUploader.filelist[0]);
		else
		{
			for(var f=0; f<oID.oUploader.filelist.length; f++)
				formdata.append(fieldname+'[]', oID.oUploader.filelist[f]);
		}

		// Send form
		xhr.send(formdata);
	}
	else
		frm.submit();
}

function fxf_fn_checkUploadProgress(uld, fieldname, up)
{
	if(up == -1)
	{
		if(oFXP.lang == 1)
			var rm='Upload wird abgebrochen...';
		else
			var rm='Canceling upload...';
		fxf_fn_drawProgressBar(up, up, rm);
	}

	if(up <= 100)
	{
		var url=fxf_fn_gProgram('check_upload', 'fieldname='+fieldname+'&up='+up);
		if(uld && oID.oUploader.filelist.length)
			url += '&fc='+oID.oUploader.filelist.length;
		url += '&lng='+oFXP.lang+fxf_fn_gParam();
//alert('url='+url);

		new Ajax.Request(url,
		{
			method:'get', asynchronous:asajax,
			onSuccess: function(transport)
			{
				var rt=transport.responseText;
//alert(rt);

				var rs=rt.split('*');
				var pc=parseInt(rs[0]);
				var rm='';
				if(rs.length > 1)
					rm=rs[1];
				var fn='';
				if(rs.length > 2)
					fn=rs[2];
				var cl=0;
				if(rs.length > 3)
					cl=parseInt(rs[3]);
				var bp=0;
				if(rs.length > 4)
					bp=parseInt(rs[4]);
//alert('rs: (fieldname='+fieldname+', up='+up+')\n\n'+rs+'\n\npc='+pc+', rm='+rm+', fn='+fn+', cl='+cl+', bp='+bp);

				$('progress_files').innerHTML=fn;
				fxf_fn_drawProgressBar(up, pc, rm);

				if((up >= 0) && (pc < 100))
					fxf_fn_checkUploadProgress.delay(0.25, uld,fieldname,pc);

/*
				if(up == -2)
				{
					var frm=$(fieldname+'_upload_form');
					if(Prototype.Browser.IE)
						frm.document.execCommand('Stop');
					else
						frm.stop();
				}
*/
			}
		});
	}
}

function fxf_fn_uploadFileDone(fieldname, utype, status)
{
//alert('fxf_fn_uploadFileDone(fieldname='+fieldname+', utype='+utype+', status='+status+')');
	oID.waitscreen.style.display='none';
	oID.waitcontent.innerHTML='';

	if((typeof fieldname != 'undefined') && fieldname && fieldname.length)
	{
		var uld=0;
		var field=$(fieldname);
		var fus=$(fieldname+'_upload_status');
		if(!fus)	// Uploader
			uld=1;
		if(((typeof utype == 'undefined') || !utype || !utype.length) && field && field.attributes && (typeof field.attributes['utype'] != 'undefined'))
			utype=field.attributes['utype'].value.toLowerCase().substr(0,3);
		// Status | Message | Filename(s) | Tempname(s) | Newname(s) | Filetype(s) | Filesize(s) | Icon(s) | Pic-Width(s) | Pic-Height(s)
		if((typeof status == 'undefined') || !status || !status.length)
			status=frames['uploader_iframe'].document.getElementsByTagName('body')[0].innerHTML;
		var ifc=status.split('|');
		var ifs=ifc[0].substr(0,1);
//alert('ifc: (uld='+uld+', utype='+utype+', status='+status+', ifs='+ifs+')\n\n'+ifc);

		if(ifs == 'O')	// OK=Success
		{
			if(fus)
				fus.innerHTML='<font color="#000000">OK</font>';
			else if(uld)
				fxf_fn_showMessage('001', ifc[1]);

			if(oFXP.tr == 217)								// Form designer
			{
				$('conf_status_div').innerHTML=ifc[0];
				fxf_fn_configIntegrate('');
			}
			else if(utype == 'pic')		// Pictures
			{
				var pfield=$(fieldname);
				if(pfield)
				{
					pfield.style.display='none';
					pfield.src=ifc[4];
					var dfield=$('div_'+fieldname);
					if(dfield)
					{
						var dw=parseInt(dfield.style.width);
						var dh=parseInt(dfield.style.height);
						var pw=0;
						var ph=0;
						if(ifc.length > 8)
							pw=parseInt(ifc[8]);
						if(ifc.length > 9)
							ph=parseInt(ifc[9]);
						var nw=pw;
						var nh=ph;
						if(dw && (pw > ph) && (pw != dw))
						{
							nw=dw;
							nh=Math.floor(nw*ph / pw);
						}
						if(dh && (ph >= pw) && (nh != dh))
						{
							nw=Math.floor(nw*dh / nh);
							nh=dh;
						}
						if(nw)
							pfield.style.width=nw+'px';
						else
							pfield.style.width='';
						if(nh)
							pfield.style.height=nh+'px';
						else
							pfield.style.height='';
						if(nh < dh)
							pfield.style.top=Math.floor((dh-nh)/2)+'px';
						else
							pfield.style.top='';
//alert('ifc[4]='+ifc[4]+'\nsrc='+pfield.src+'\n\ndw='+dw+', dh='+dh+'\npw='+pw+', ph='+ph+'\n\nnw='+nw+', nh='+nh+'\n\npfield.style.width='+pfield.style.width+'\npfield.style.height='+pfield.style.height+'\npfield.style.top='+pfield.style.top);
					}
					pfield.style.display='';
					if(dfield)
					{
						if(oFXP.action == 3)
							dfield.setOpacity(1.0);
						else
							dfield.setOpacity(Math.max(dfield.getOpacity(),0.50));
					}
				}
			}
			else if(utype == 'doc')	// Documents
			{
				var dfield=$('div_'+fieldname);
				var sfield=$('span_'+fieldname);
				if(dfield && sfield)
				{
					var t=ifc[2];
					if((ifc.length > 7) && ifc[7].length)
						t='<img src="GFX/ico_'+ifc[7]+'.png" align="top" />&nbsp;'+t;
					if(sfield.innerHTML.substr(0,3) == '<i>')	// New document
					{
						sfield.setOpacity(Math.max(sfield.getOpacity(),0.50));
						sfield.innerHTML=t;
					}
					else										// Document changed
						dfield.innerHTML='<span id="span_'+fieldname+'" name="span_'+fieldname+'" style="opacity:0.22;">'+sfield.innerHTML+'</span>&nbsp;&nbsp;<span class="lightergrey">&rarr;</span>&nbsp;&nbsp;'+t;
				}
			}
		}
		else if(fus)
			fus.innerHTML='<font id="fus_err" class="red" tooltip="<font class=red>Err-'+ifc[1]+'</font>" style="cursor:help;">'+ifc[0]+'</font>';
		else if(uld)
			fxf_fn_showMessage('100', ifc[1]);

		if(oFXP.tr == 110)	// Document manager
		{
			tSet.dms.save=[];
			tSet.dms.rights=['',''];
			fxf_fn_dmsRead('', tSet.dms.aspath, tSet.dms.aseid, '');
		}
	}
}

function fxf_fn_checkLine(mc, line, action)
{
var cdbg='fxf_fn_checkLine(mc='+mc+', line='+line+', action='+action+')\n\n';

	var cchanged='';
	var ca=[$('Anzeigen['+mc+']['+line+']'), $('Anlegen['+mc+']['+line+']'), $('Aendern['+mc+']['+line+']'), $('Loeschen['+mc+']['+line+']')];

	var all=$('alle_auswaehlen['+mc+']['+line+']');
	var frm='';
	var cbr='';
	var sa=0;
	if(all)
	{
		if(all.attributes)
		{
			if(typeof all.attributes['fxform'] != 'undefined')
				frm=all.attributes['fxform'].value;
			if(typeof all.attributes['cbrem'] != 'undefined')
				cbr=all.attributes['cbrem'].value;
		}
		if(!action)
		{
			if(all.checked)
				sa=1;
			else
				sa=-1;
		}
	}
cdbg += 'all='+all+'\n -> frm='+frm+'\nsa='+sa+', cbr='+cbr+'\n\n';

	var cbm='';
	var cba='';
	for(var c=0; c<ca.length; c++)
	{
		if(ca[c])
		{
			cbm += '1';
			if(ca[c].checked)
				cba += '1';
			else
				cba += '0';
		}
		else
		{
			cbm += '0';
			cba += '0';
		}
	}
cdbg += ' -> cbm='+cbm+', cba='+cba+'\n\n';

	var chk=$('Checkbox['+mc+']['+line+']');
	var cbs='';
	if(chk && (action < 0))
	{
		if(!chk.checked)
			cbs='0000';
		else if(cbr.length >= 4)
			cbs=cbr;
		else
		{
			cbs=cba;
			if(cbs == '0000')
				cbs='1111';
		}
	}
cdbg += 'chk='+chk+'\n -> cbs='+cbs+'\n\n';

	var cbn='';
	for(var c=0; c<ca.length; c++)
	{
		if(ca[c])
		{
			if(sa != 0)
				ca[c].checked=all.checked;
			else if(cbs.length)
			{
				if(cbs.substr(c,1) == '1')
					ca[c].checked=true;
				else
					ca[c].checked=false;
			}
			if(ca[c].checked)
				cbn += '1';
			else
				cbn += '0';

			if(cchanged.length)
				cchanged += '|';
			if(ca[c].checked)
				cchanged += '1';
			else
				cchanged += '0';
			cchanged += ca[c].id;
cdbg += c+': '+ca[c]+'\n';
		}
		else
			cbn += '0';
	}
cdbg += '\n -> cbn='+cbn+'\n\n';

	if(all)
	{
		if(cbs.length)
			all.setAttribute('cbrem', cba);
		else
			all.setAttribute('cbrem', cbn);

		if(cbn == cbm)
			all.checked=true;
		else
			all.checked=false;

		if(cchanged.length)
			cchanged += '|';
		if(all.checked)
			cchanged += '1';
		else
			cchanged += '0';
		cchanged += all.id;

cdbg += '\n -> all.attributes[\'cbrem\'].value='+all.attributes['cbrem'].value+'\n\n';
	}

	if(chk)
	{
		if(cbn == '0000')
			chk.checked=false;
		else
			chk.checked=true;

		if(cchanged.length)
			cchanged += '|';
		if(chk.checked)
			cchanged += '1';
		else
			cchanged += '0';
		cchanged += chk.id;
	}
cdbg += '-> cchanged='+cchanged;
//alert(cdbg);

	if(oFXP.tr == 110)
		fxf_fn_dmsRights('add');
	else if(cchanged.length)
	{
fxf_fn_writeDebug('info', 'checkLine: frm='+frm+', cchanged='+cchanged);
		var url=fxf_fn_worker('setcb','keep_post=1&frm='+frm);
		var pst='cbs='+cchanged;
		fxf_fn_saveCBLine.delay(oFXP.ldelay, url,pst);
	}
}

function fxf_fn_saveCBLine(url,pst)
{
//alert('url='+url+'\npst='+pst);
	new Ajax.Request(url,
	{
		method:'post', postBody:pst, asynchronous:true,
		onSuccess: function(transport)
		{
fxf_fn_writeDebug('log', transport.responseText);
		}
	});
}

function fxf_fn_drawLanguage(lar)
{
	if(lar.length > 0)
	{
		var ls='';
		var lp='<table border="0" cellpadding="0" cellspacing="2">';
		for(li=0; li<lar.length; li++)
		{
			if(lar[li].id == oFXP.lang)
				ls='GFX/flg_'+fxf_fn_addLeadingZeros(lar[li].id,2)+'.png';

			var cls='black';
			var idst=' id="fxtasklps'+lar[li].id+'" style="cursor:pointer;';
			if(lar[li].id == oFXP.lang)
			{
				cls='blue';
				idst=' style="cursor:default;';
			}
			lp += '<tr'+idst+'"><td'+idst+'"><img src="GFX/flg_'+fxf_fn_addLeadingZeros(lar[li].id,2)+'b.png"'+idst+'width:44px;height:28px;"></td><td>&nbsp;<span class="'+cls+'"'+idst+'">'+lar[li].text+'</span></td></tr>';
		}
		lp += '</table>';

		oID.fxtaskli.src=ls;
		fxf_fn_dispLang(true);

		oID.fxtasklp.innerHTML=lp;
	}
	else
		oID.fxtaskl.style.display='none';
	oID.fxtasklp.style.display='none';
}

function fxf_fn_setLanguage(nlang)
{
	new Ajax.Request(fxf_fn_worker('setnewlang','keep_post=1'),
	{
		method:'post', postBody:'newlang='+nlang, asynchronous:true,
		onSuccess: function(transport)
		{
			fxf_fn_reload();
		},
		onFailure: function()
		{
			fxf_fn_waitScreen('');
		}
	});
}

function fxf_fn_changeSetting(str,svar,sval)
{
	new Ajax.Request(fxf_fn_worker('changesetting','keep_post=1'),
	{
		method:'post', postBody:'str='+str+'&newsetting='+svar+'&value='+sval, asynchronous:true,
		onSuccess: function(transport) {}
	});
}

function fxf_fn_saveUserSetting(sval)
{
	new Ajax.Request(fxf_fn_worker('usersetting','keep_post=1'),
	{
		method:'post', postBody:'value='+sval, asynchronous:true,
		onSuccess: function(transport) {}
	});
}

function fxf_fn_saveFields(post,param)
{
fxf_fn_writeDebug('info', 'saveFields: post='+post+', param='+param);
	var get='keep_post=1';
	if(param && param.length)
		get += '&'+param;
	var url=fxf_fn_worker('sfields',get);
//alert('url='+url+'\npost='+post);
	fxf_fn_ajaxSave(url,post);
}

function fxf_fn_saveListEntries(post,param)
{
fxf_fn_writeDebug('info', 'saveListEntries: post='+post+', param='+param);
	var get='keep_post=1';
	if(param && param.length)
		get += '&'+param;
	var url=fxf_fn_worker('slist',get);
//alert('url='+url+'\npost='+post);
	fxf_fn_ajaxSave(url,post);
}

function fxf_fn_fieldHidden(id,value,form)
{
	var html = '<input type="hidden" id="'+id+'" name="'+id+'" fxform="'+form+'" value="'+value+'">';

	return html;
}

function fxf_fn_prjcatClose()
{
	var prjcatpc = $('prjcatpc');
	if(prjcatpc)
	{
		$(document.body).removeChild(prjcatpc);
		oID.iact.style.display='none';
	}
}

function fxf_fn_prjcatGetBG(b,disabled)
{
	var b0='url(GFX/pcb.png) no-repeat top left';
	if(disabled)
		b0='transparent';
	if(typeof b !== 'number')
	{
		if(!b.length)
			return b0;
		else
			b=parseInt(b);
	}
	if(b <= 0)
		return b0;

	var bd=$('prjcat_bgcolors');
	if(bd)
		var ca=bd.innerHTML.split('|');
	else
		return b0;

	if(b >= ca.length)
		b=ca.length-1;

	var c='#'+ca[b];
//alert('fxf_fn_prjcatGetBG('+b+','+disabled+') -> '+c);

	return c;
}

function fxf_fn_prjcatGetBC(b)
{
	if(!b.length || (b.substr(0,1) == 't') || (b.substr(0,1) == 'u'))
		return 0;

	if(b.substr(0,1) == '#')
		b=b.substr(1);

	var bd=$('prjcat_bgcolors');
	if(bd)
		var ca=bd.innerHTML.split('|');
	else
		return 0;

	var c=0;
	for(var i=0; i<ca.length; i++)
	{
		if(ca[i] == b)
		{
			c=i;
			break;
		}
	}
//alert('fxf_fn_prjcatGetBC('+b+') -> '+c);

	return c;
}

function fxf_fn_prjcatChangeBG(gc,cc)
{
	var prjcatpc = $('prjcatpc');
	if(prjcatpc)
	{
		fxf_fn_prjcatClose();
		return;
	}

	dleft 	= mcx-12;
	dtop  	= mcy;
	dheight	= 90;

	var bd=$('prjcat_bgcolors');
	if(bd)
	{
		var ca=bd.innerHTML.split('|');
		dheight = 70 + ca.length*20;
	}

	if(dtop+dheight+50 > dim.sd.pheight)
		dtop -= (dheight-20);
	else
		dtop -= 20;

	if(cc > 0)
	{
		var spi='cp'+gc+'_'+cc;
		var sgi='cg'+gc+'_'+cc;
		var svi='cv'+gc+'_'+cc;
	}
	else
	{
		var spi='gp'+gc;
		var sgi='gg'+gc;
		var svi='gv'+gc;
	}

	var spos = parseInt($(spi).value);
	var sbg = parseInt($(sgi).value);
	var sval = $(svi).value;

	oID.iact.style.display='';

	var cont = new Element('div', {id:'prjcatpc', style:'position:absolute;left:'+dleft+'px;top:'+dtop+'px;width:auto;height:auto;border:0;background-color:transparent;'});
	$(document.body).insert(cont);

	var url=fxf_fn_gProgram('prj_kategorie', 'language='+oFXP.lang+'&gc='+gc+'&cc='+cc+fxf_fn_gParam());
//alert(url+'\n\npost:\nspos='+spos+', sbg='+sbg+', sval='+sval);
	new Ajax.Request(url,
	{
		method:'post', postBody:'spos='+spos+'&sbg='+sbg+'&sval='+encodeURIComponent(sval), asynchronous:true,

		onSuccess: function(transport)
		{
			cont.innerHTML = transport.responseText;
		},

		onFailure: function(failure)
		{
			fxf_fn_prjcatChangeBGClose();
		}
	});
}

function fxf_fn_prjcatSetBG(gc,cc,c)
{
	fxf_fn_prjcatClose();
	if(cc > 0)
		var f='cg'+gc+'_'+cc;
	else
		var f='gg'+gc;
	var fi=$(f);
	if(fi)
	{
		var bd=$('prjcat_bgcolors');
		if(bd)
			var ca=bd.innerHTML.split('|');
		else
			var ca=['GFX/pcb.png) no-repeat top left'];

		if(typeof c !== 'number')
			c='';
		else
		{
			c=parseInt(c);
			if(isNaN(c))
				c='';
			else
			{
				if(c >= ca.length)
					c=ca.length-1;
				if(c <= 0)
					c='';
			}
		}

		fi.value=c;
		fi.style.background=fxf_fn_prjcatGetBG(c,false);

		fxf_fn_saveFields(f+'=tx'+c, 'frm='+tSet.form);
	}
}

function fxf_fn_addJavaScript(otr)
{
//alert('fxf_fn_addJavaScript('+otr+') - oFXP.tr='+oFXP.tr);
	var ams=0;
	var tfa=false;
	var jsa=[], jta=[], jia=[], jpa=[];
	var jis='|';
	var trjsi=$('trjsi');
	if(trjsi)
	{
//alert('trjsi='+trjsi);
		var ja=trjsi.innerHTML.split('!-FXJS-!');
		if(ja && ja.length)
		{
			for(var j=0; j<ja.length; j++)
			{
				jsa[j]=ja[j].substr(0,1);
				jta[j]=ja[j].substr(1,3);
				if(jta[j] == '2fa')
				{
					jia[j]='2FA';
					tfa=true;
				}
				else
					jia[j]=parseInt(ja[j].substr(1,3));
				jpa[j]=ja[j].substr(4);

				jis += jia[j]+'|';
			}
		}
//alert('ja='+ja+'\n\njsa='+jsa+'\njta='+jta+'\njia='+jia+'\njpa='+jpa+'\n\njis='+jis);
	}

	var trjst=$('trjst_'+oFXP.tr);
	if(!trjst && (tfa || ((oFXP.tr > 0) && (oFXP.tr != otr) && jta.length)))
	{
		// Insert new additional div/javascript(s)
		var body=document.getElementsByTagName('body')[0];
//alert('body='+body);
		if(body)
		{
			// Div
			var d=document.createElement('div');
			d.id='trjst_'+oFXP.tr;
			d.style.display='none';
			d.innerHTML=jis;
			body.appendChild(d);
//alert('Append div:\nid='+d.id+'\ninnerHTML='+d.innerHTML);

			// JavaScript(s)
			for(var j=0; j<jta.length; j++)
			{
				var jse=$('trjsa_'+jta[j]);
				if(!jse)
				{
					var s=document.createElement('script');
					s.id='trjsa_'+jta[j];
					s.type='text/javascript';
					if(jsa[j] == '#')
						s.src=jpa[j];
					else
					{
						var jst=jpa[j].replace(/\(\(lt\)\)/g, '<').replace(/\(\(gt\)\)/g, '>').replace(/\(\(and\)\)/g, '&').replace(/\(\(asterisk\)\)/g, '*').replace(/\(\(plus\)\)/g, '+');
						s.innerHTML=jst;
					}
					body.appendChild(s);
//alert('Append javascript ['+j+']:\njsa='+jsa[j]+', jta='+jta[j]+', id='+s.id+'\nsrc='+s.src+'\ninnerHTML='+s.innerHTML);
				}
			}
			trjsi.innerHTML='';
		}
	}

	// Execute additional javascript init function(s)
	trjsa=$$('[id^="trjsa_"]');
//alert('trjsa='+trjsa);
	if(trjsa && trjsa.length)
	{
		var jis='';
		var trjst=$('trjst_'+oFXP.tr);
		if(trjst)
			jis=trjst.innerHTML;

		for(var j=0; j<trjsa.length; j++)
		{
			var jstr=trjsa[j].id.substr(6,3);
			var jint=parseInt(jstr);
//alert('jstr='+jstr+', jint='+jint);
			// Call init function?
			if((!oFXP.tr && (jstr == '2fa')) || ((oFXP.tr == jint) || (jis.indexOf('|'+jint+'|') >= 0)))
			{
				ams += 1000;
//alert(j+': id='+trjsa[j].id+', jstr='+jstr+' -> fxf_fn_callFunction(\'fxf_fn_init'+jint+'\');');
				if(jstr == '2fa')
					fxf_fn_callFunction('fxf_fn_init2FA');
				else
					fxf_fn_callFunction('fxf_fn_init'+jint);
			}
//			else alert(j+': id='+trjsa[j].id+' -> SKIP: '+jint);
		}
	}

	return ams;
}

function fxf_fn_callFunction(f, c, e,a)
{
	if(typeof(c) == 'undefined')
		c=0.0;
	else
		c=parseFloat(c);
	if(typeof(e) == 'undefined')
		e='';
	if(typeof(a) == 'undefined')
		a='';

	// Check if function exists
	var tf=eval('typeof '+f);
	if(tf === 'function')
	{
		var x=e+f+'('+a+');';
//alert('Function EXISTS -> eval '+x);
		eval(x);
	}
	else if(c < 5.0)	// Try for max. 5 seconds
	{
//alert(c+': Function "'+f+'" NOT FOUND!!!');
		fxf_fn_callFunction.delay(0.1, f, c+0.1, e,a);
	}
}

function fxf_fn_hexToRgb(hex_string, percent_, default_)
{
	if(percent_ == undefined)
		percent_ = true;
	if(default_ == undefined)
		default_ = null;

	if(hex_string.substr(0, 1) == '#')
		hex_string = hex_string.substr(1);

	var r, g, b;
	if(hex_string.length == 3)
	{
		r = hex_string.substr(0, 1); r += r;
		g = hex_string.substr(1, 1); g += g;
		b = hex_string.substr(2, 1); b += b;
	}
	else if(hex_string.length == 6)
	{
		r = hex_string.substr(0, 2);
		g = hex_string.substr(2, 2);
		b = hex_string.substr(4, 2);
	}
	else
        return default_;

	r = parseInt(r, 16);
	g = parseInt(g, 16);
	b = parseInt(b, 16);
	if(isNaN(r) || isNaN(g) || isNaN(b))
		return default_;
	else if(percent_)
		return {r: r/255, g: g/255, b: b/255};
	else
		return {r: r, g: g, b: b};
}

function fxf_fn_rgbToHex(r, g, b, includeHash)
{
	var rgb=r.toString();
	var io=rgb.indexOf('rgb(');
	if(io >= 0)
	{
		var rgba=rgb.substr(io+4).split(',');
		r=0;
		if(rgba.length > 0)
			r=parseInt(rgba[0].trim());
		g=0;
		if(rgba.length > 1)
			g=parseInt(rgba[1].trim());
		b=0;
		if(rgba.length > 2)
			b=parseInt(rgba[2].trim());
//alert('rgba: (rgb='+rgb+', io='+io+')\n-> ['+rgba+']\n\nr='+r+', g='+g+', b='+b);
	}

	r=Math.min(255,Math.max(0,parseInt(r)));
	g=Math.min(255,Math.max(0,parseInt(g)));
	b=Math.min(255,Math.max(0,parseInt(b)));
	if(includeHash == undefined)
        includeHash = true;

	r=r.toString(16);
	if(r.length == 1)
		r='0'+r;
	g=g.toString(16);
	if(g.length == 1)
		g='0'+g;
	b=b.toString(16);
	if(b.length == 1)
		b='0'+b;

	return ((includeHash ? '#' : '') + r + g + b).toUpperCase();
}

function fxf_fn_fixPNG(myImage)
{
	var arVersion = navigator.appVersion.split("MSIE");
	var version = parseFloat(arVersion[1]);
	if ((version >= 5.5) && (version < 7) && (document.body.filters))
	{
		var node = document.createElement('span');

		node.id = myImage.id;
		node.className = myImage.className;
		node.title = myImage.title;
		node.style.cssText = myImage.style.cssText;
		node.style.setAttribute('filter', "progid:DXImageTransform.Microsoft.AlphaImageLoader(src=\'" + myImage.src + "\', sizingMethod='scale')");
		node.style.fontSize = '0';
		node.style.width = myImage.width.toString() + 'px';
		node.style.height = myImage.height.toString() + 'px';
		node.style.display = 'inline-block';
	}
	else
		var node = myImage.cloneNode(false);

	node.style.left = '0';
	node.style.top = '0';

	return node;
}

function fxf_fn_trackDrag(node, handler)
{
	function fixCoords(cx, cy)
	{
		var nodePageCoords = fxf_fn_pageCoords(node);

		cx = (cx - nodePageCoords.cx) + oID.fxwork.scrollLeft - 2;
		cx = Math.max(0, Math.min(cx, 199));
		cy = (cy - nodePageCoords.cy) + oID.fxwork.scrollTop - 2;
		cy = Math.max(0, Math.min(cy, 199));

		return {cx: cx, cy: cy};
	}

	function mouseDown(ev)
	{
		var coords = fixCoords(ev.clientX, ev.clientY);
		var lastX = coords.cx;
		var lastY = coords.cy;

		handler(coords.cx, coords.cy);

		function moveHandler(ev)
		{
			var coords = fixCoords(ev.clientX, ev.clientY);

			if((coords.cx != lastX) || (coords.cy != lastY))
			{
				lastX = coords.cx;
				lastY = coords.cy;
				handler(coords.cx, coords.cy);
			}
		}

		function upHandler(ev)
		{
			fxf_fn_myRemoveEventListener(document, 'mouseup', upHandler);
			fxf_fn_myRemoveEventListener(document, 'mousemove', moveHandler);
			fxf_fn_myAddEventListener(node, 'mousedown', mouseDown);
		}

		fxf_fn_myAddEventListener(document, 'mouseup', upHandler);
		fxf_fn_myAddEventListener(document, 'mousemove', moveHandler);
		fxf_fn_myRemoveEventListener(node, 'mousedown', mouseDown);

		if(ev.preventDefault)
			ev.preventDefault();
	}

	fxf_fn_myAddEventListener(node, 'mousedown', mouseDown);
	node.onmousedown = function(e) { return false; };
	node.onselectstart = function(e) { return false; };
	node.ondragstart = function(e) { return false; };
}

function fxf_fn_findEventListener(node, event, handler)
{
	var i;
	for(i in eventListeners)
	{
		if(eventListeners[i].node == node && eventListeners[i].event == event && eventListeners[i].handler == handler)
			return i;
	}
	return null;
}

function fxf_fn_myAddEventListener(node, event, handler)
{
	if(fxf_fn_findEventListener(node, event, handler) != null)
		return;
	if(!node.addEventListener)
		node.attachEvent('on' + event, handler);
    else
		node.addEventListener(event, handler, false);

	eventListeners.push({node: node, event: event, handler: handler});
}

function fxf_fn_removeEventListenerIndex(index)
{
	var eventListener = eventListeners[index];
	delete eventListeners[index];
	if(!eventListener.node.removeEventListener)
		eventListener.node.detachEvent('on' + eventListener.event, eventListener.handler);
    else
		eventListener.node.removeEventListener(eventListener.event, eventListener.handler, false);
}

function fxf_fn_myRemoveEventListener(node, event, handler)
{
	fxf_fn_removeEventListenerIndex(fxf_fn_findEventListener(node, event, handler));
}

function fxf_fn_cleanupEventListeners()
{
    var i;
    for(i=eventListeners.length; i>0; i--)
    {
        if(eventListeners[i] != undefined)
            fxf_fn_removeEventListenerIndex(i);
    }
}

// This copyright statement applies to the following two functions,
// which are taken from MochiKit.
//
// Copyright 2005 Bob Ippolito <bob@redivi.com>
//
// Permission is hereby granted, free of charge, to any person obtaining
// a copy of this software and associated documentation files (the
// "Software"), to deal in the Software without restriction, including
// without limitation the rights to use, copy, modify, merge, publish,
// distribute, sublicense, and/or sell copies of the Software, and to
// permit persons to whom the Software is furnished to do so, subject
// to the following conditions:
//
// The above copyright notice and this permission notice shall be
// included in all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
// BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
// ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
// CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

function fxf_fn_hsvToRgb(hue, saturation, value)
{
    var red;
    var green;
    var blue;
    if (value == 0.0)
    {
        red = 0;
        green = 0;
        blue = 0;
    }
    else
    {
        var i = Math.floor(hue * 6);
        var f = (hue * 6) - i;
        var p = value * (1 - saturation);
        var q = value * (1 - (saturation * f));
        var t = value * (1 - (saturation * (1 - f)));
        switch (i)
        {
            case 1: red = q; green = value; blue = p; break;
            case 2: red = p; green = value; blue = t; break;
            case 3: red = p; green = q; blue = value; break;
            case 4: red = t; green = p; blue = value; break;
            case 5: red = value; green = p; blue = q; break;
            case 6: // fall through
            case 0: red = value; green = t; blue = p; break;
        }
    }
    return {r: red, g: green, b: blue};
}

function fxf_fn_rgbToHsv(red, green, blue)
{
    var max = Math.max(Math.max(red, green), blue);
    var min = Math.min(Math.min(red, green), blue);
    var hue;
    var saturation;
    var value = max;

    if(min == max)
    {
        hue = 0;
        saturation = 0;
    }
    else
    {
        var delta = (max - min);
        saturation = delta / max;
        if (red == max)
        {
            hue = (green - blue) / delta;
        }
        else if (green == max)
        {
            hue = 2 + ((blue - red) / delta);
        }
        else
        {
            hue = 4 + ((red - green) / delta);
        }
        hue /= 6;
        if (hue < 0)
        {
            hue += 1;
        }
        if (hue > 1)
        {
            hue -= 1;
        }
    }
    return {
        h: hue,
        s: saturation,
        v: value
    };
}

function fxf_fn_pageCoords(node)
{
    var cx = node.offsetLeft;
    var cy = node.offsetTop;
    var parent = node.offsetParent;
    while (parent != null)
    {
        cx += parent.offsetLeft;
        cy += parent.offsetTop;
        parent = parent.offsetParent;
    }
    return {cx: cx, cy: cy};
}

function fxf_fn_makeColorSelector(inputBox)
{
	var huePositionImg = document.createElement('img');
	huePositionImg.galleryImg = false;
	huePositionImg.width = 35;
	huePositionImg.height = 11;
	huePositionImg.src = 'GFX/cp_a.png';
	huePositionImg.style.position = 'absolute';

	var hueSelectorImg = document.createElement('img');
	hueSelectorImg.galleryImg = false;
	hueSelectorImg.width = 35;
	hueSelectorImg.height = 200;
	hueSelectorImg.src = 'GFX/cp_h.png';
	hueSelectorImg.style.display = 'block';
	hueSelectorImg.style.boxShadow = '1px 1px 7px #444444';

	var satValImg = document.createElement('img');
	satValImg.galleryImg = false;
	satValImg.width = 200;
	satValImg.height = 200;
	satValImg.src = 'GFX/cp_s.png';
	satValImg.style.display = 'block';
	satValImg.style.boxShadow = '1px 1px 7px #444444';

	var crossHairsImg = document.createElement('img');
	crossHairsImg.galleryImg = false;
	crossHairsImg.width = 21;
	crossHairsImg.height = 21;
	crossHairsImg.src = 'GFX/cp_c.png';
	crossHairsImg.style.position = 'absolute';

    var rgb, hsv;

	var colorSelectorDiv = new Element('div', {id:'conf_colorselector', style:'position:relative;width:248px;height:244px;'});

	var satValDiv = new Element('div', {id:'conf_satval', style:'position:absolute;left:0;top:0;width:200px;height:200px;'});
    var newSatValImg = fxf_fn_fixPNG(satValImg);
    satValDiv.appendChild(newSatValImg);
    var crossHairs = crossHairsImg.cloneNode(false);
    satValDiv.appendChild(crossHairs);
    fxf_fn_trackDrag(satValDiv, satValDragged);
    colorSelectorDiv.appendChild(satValDiv);

	var hueDiv = new Element('div', {id:'conf_hue', style:'position:absolute;right:0;top:0;width:35px;height:200px;'});
	var huePos = fxf_fn_fixPNG(huePositionImg);
	hueDiv.appendChild(hueSelectorImg.cloneNode(false));
	hueDiv.appendChild(huePos);

	fxf_fn_trackDrag(hueDiv, hueDragged);
	colorSelectorDiv.appendChild(hueDiv);

	var previewDiv = new Element('div', {id:'conf_color_preview', style:'position:absolute;left:0;bottom:0;width:135px;height:28px;border:1px solid #666666;box-shadow:-1px -1px 6px #000000 inset;'});
	colorSelectorDiv.appendChild(previewDiv);

	fxf_fn_myAddEventListener(inputBox, 'change', inputBoxChanged);
	inputBox.style.height = '28px';
	inputBox.style.width = '110px';
	inputBox.style.position = 'absolute';
	inputBox.style.right = '0';
	inputBox.style.bottom = '0';
	inputBox.style.fontSize = '22px';
	inputBox.style.border = '1px solid #666666';
	inputBox.style.boxShadow = '1px 1px 6px #888888 inset';
	colorSelectorDiv.appendChild(inputBox);

	inputBoxChanged();

    function satValDragged(cx, cy)
    {
        hsv.s = 1-(cy/199);
        hsv.v = (cx/199);
        hsvChanged();
    }
	function hueDragged(cx, cy)
	{
		hsv.h = cy/199;
		hsvChanged();
	}
    function colorChanged()
    {
        var hex=fxf_fn_rgbToHex(rgb.r*255, rgb.g*255, rgb.b*255);
        var hueRgb=fxf_fn_hsvToRgb(hsv.h, 1, 1);
        var hueHex=fxf_fn_rgbToHex(hueRgb.r*255, hueRgb.g*255, hueRgb.b*255);
        previewDiv.style.background=hex;
        inputBox.value=hex;
        satValDiv.style.background=hueHex;
        crossHairs.style.left=((hsv.v*199)-10).toString() + 'px';
        crossHairs.style.top=(((1-hsv.s)*199)-10).toString() + 'px';
        huePos.style.top=((hsv.h*199)-5).toString() + 'px';
    }
    function rgbChanged()
    {
        hsv=fxf_fn_rgbToHsv(rgb.r, rgb.g, rgb.b);
        colorChanged();
    }
    function hsvChanged()
    {
        rgb=fxf_fn_hsvToRgb(hsv.h, hsv.s, hsv.v);
        colorChanged();
    }
	function inputBoxChanged()
	{
		rgb=fxf_fn_hexToRgb(inputBox.value, true, {r: 0, g: 0, b: 0});
		rgbChanged();
	}

	return colorSelectorDiv;
}

// Columns Popup
function fxf_fn_columnsPopup(element, event)
{
	var mn=element.id.substr(7);
	var ww=dim.wd.width;
	var wh=dim.wd.height;
	var fs='';
	var rcd=$('rcfields_'+mn);
	if(rcd)
	{
		var rcf=rcd.innerHTML.split('|');
		if(rcf && rcf.length)
		{
			for(var f=0; f<rcf.length; f++)
			{
				var el=$('rc_'+mn+'_'+rcf[f]);
				if(el)
					fs += el.value;
				else
					fs += '0';
			}
		}
	}

	var href=fxf_fn_gProgram('popup_col', 'mn='+mn+'&ww='+ww+'&wh='+wh+'&fs='+fs+fxf_fn_gParam());
//alert('columnsPopup: href='+href);
	fxf_fn_fxLink(element,href);
}

function fxf_fn_rcAction(mn,md,field)
{
//alert('fxf_fn_rcAction('+mn+','+md+','+field+')');
	var nsv='';

	// 0:Default + 1:All
	if((md == 0) || (md == 1))
	{
		var rcd=$('rcdefall_'+mn);
		if(rcd)
		{
			var rcf=rcd.innerHTML.split('|');
			nsv=rcf[md];
		}
	}
	// -2: Reverse + -1: Field changed
	else if(md < 0)
	{
		var rcd=$('rcfields_'+mn);
		if(rcd)
		{
			var rcf=rcd.innerHTML.split('|');
			var cc=0;
			var rl=rcf.length;
			var pf='';
			while(rl && (cc<rl))
			{
				var el=$('rc_'+mn+'_'+rcf[cc]);
				if(el)
				{
					var vi=el.value;
					if((vi == '0') || (vi == '1'))
					{
						if(md == -1)
						{
							if(field && field.length && (rcf[cc] == field))
							{
								var pc=$('rccb_'+mn+'_'+rcf[cc]);
								if(pc)
								{
									if(pc.checked)
										vi='1';
									else
										vi='0';
								}
							}
						}
						else if(vi == '0')
							vi='1';
						else
							vi='0';
					}
					nsv += vi;
				}
				cc++;
			}
		}
	}
//alert('fxf_fn_rcAction('+mn+','+md+')\nnsv='+nsv);

	fxf_fn_rcSet(mn,nsv);
}

function fxf_fn_rcSelect(element,mn)
{
	var nsv=fxf_fn_getSelectedValue(element).value;
	var io=nsv.indexOf('_');
	if(io > -1)
		nsv=nsv.substr(io+1);

	fxf_fn_rcSet(mn,nsv);
}

function fxf_fn_rcSet(mn,nsv)
{
//alert('fxf_fn_rcSet('+mn+', '+nsv+')');
	if(!nsv || !nsv.length)
		return;

	var rcd=$('rcfields_'+mn);
	if(rcd)
	{
		var rcf=rcd.innerHTML.split('|');
		var cc=0;
		var rl=rcf.length;
		var pf='';
		var frm='';
		var mcp='';
		while(rl && (cc<rl))
		{
			var el=$('rc_'+mn+'_'+rcf[cc]);
			var pc=$('rccb_'+mn+'_'+rcf[cc]);
			if(el && (cc < nsv.length))
			{
				el.value=nsv.substr(cc,1);
				if(pf.length)
					pf += '&';
				pf += el.id+'=in'+el.value;

				if(pc)
				{
					if(el.value % 2)
						pc.checked=true;
					else
						pc.checked=false;
				}

				if(oFXP.tr == 189)	// Matrix: Hide column
				{
					mcp=$('dh_'+rcf[cc]);	// Header
					if(mcp)
					{
						if(el.value % 2)
							mcp.style.display='';
						else
							mcp.style.display='none';
					}
					mcp=$('dw_'+rcf[cc]);	// Data
					if(mcp)
					{
						if(el.value % 2)
							mcp.style.display='';
						else
							mcp.style.display='none';
					}
				}

				if(!frm.length && el.attributes && el.attributes['fxform'])
					frm=el.attributes['fxform'].value;
			}
			cc++;
		}

		if(pf.length)
		{
//alert('saveFields: frm='+frm+'\n'+pf);
			fxf_fn_saveFields(pf,'frm='+frm);
		}

		var melement=fxf_fn_getField($('rcmain_'+mn),true);
		var pelement=fxf_fn_getField($('rcsel_'+mn),true);
		if(melement)
			fxf_fn_changeSelectedValue(melement,nsv,true);
		if(pelement)
			fxf_fn_changeSelectedValue(pelement,nsv,true);

		if(oFXP.tr == 189)	// Matrix: Hide column
			fxf_fn_resizeMatrix();
	}
}

function fxf_fn_toggleGroups(group)
{
	var elem=$('ipg'+group);
	var ipg=elem.src.substr(0, elem.src.length-6);
	var img=elem.src.substr(elem.src.length-6, 2);
	var isf=elem.src.substr(elem.src.length-4, 4);
//alert('fxf_fn_toggleGroups('+group+')\n\ne='+elem+', elem.src='+elem.src+'\nipg='+ipg+', img='+img+', isf='+isf);
	var enl=true;
	if(img == 'op')
		img='cl';
	else
	{
		img='op';
		enl=false;
	}
	elem.src=ipg+img+isf;

	var cnt=0;
	var id_tr=null;
	while(true)
	{
		id_tr=$('tr'+group+'_'+cnt);
		if(!id_tr)
			break;

		if(enl)
			id_tr.style.display='';
		else
			id_tr.style.display='none';

		cnt++;
	}
}

function fxf_fn_adjustAbsent()
{
	var id_chk=fxf_fn_getElement('Fehltagsart');
	if(id_chk)
	{
		fxf_fn_waitScreen('#');
		var oatype=fxf_fn_getSelectedValue(id_chk).value;
		fxf_fn_changeSelectedValue.delay(oFXP.mdelay, id_chk,oatype,true);
	}
}

function fxf_fn_adjustAbsent2()
{
	var id_sel		= fxf_fn_getElement('ganztaegig');
	var id_sel_lit	= fxf_fn_getElement('td_lit_ganztaegig');
	var id_sel_img	= fxf_fn_getElement('img_ganztaegig');

	var id_end		= fxf_fn_getElement('Soll_End_Dtm');
	var id_end_lit	= fxf_fn_getElement('td_lit_Soll_End_Dtm');
	var id_end_img	= fxf_fn_getElement('img_Soll_End_Dtm');

	var id_hrs		= fxf_fn_getElement('stunden');

	var id_chk		= fxf_fn_getElement('Fehltagsart');

	if(id_sel && id_chk)
	{
		var atype=parseInt(fxf_fn_getSelectedValue(id_chk).value);
//alert('atype='+atype);

		// 1940: "Flextime application" + 1941: "Flextime"
		if((atype == 1940) || (atype == 1941))
		{
			if(id_sel)
				id_sel.style.display='none';
			if(id_sel_lit)
				id_sel_lit.innerHTML='';
			if(id_sel_img)
				id_sel_img.style.display='none';

			if(oFXP.action == 3)
			{
				if(id_end)
					id_end.style.display='none';
				if(id_end_lit)
					id_end_lit.innerHTML='';
				if(id_end_img)
					id_end_img.style.display='none';
			}
			else
			{
				if(id_end)
					id_end.style.display='';
				if(id_end_lit)
					id_end_lit.innerHTML='<span class="lit">'+oSet.lit_endtime+':&nbsp;</span>';
				if(id_end_img)
					id_end_img.style.display='';
			}

			if(id_hrs)
				id_hrs.style.display='none';
		}
		// 1942: "Overtime outpayment application" + 1943: "Overtime outpayment"
		else if((atype == 1942) || (atype == 1943))
		{
			if(id_sel)
				id_sel.style.display='none';
			if(id_sel_lit)
				id_sel_lit.innerHTML='<span class="lit">'+oSet.lit_hours+':&nbsp;</span>';
			if(id_sel_img)
				id_sel_img.style.display='none';

			if(id_end)
				id_end.style.display='none';
			if(id_end_lit)
				id_end_lit.innerHTML='';
			if(id_end_img)
				id_end_img.style.display='none';

			if(id_hrs)
				id_hrs.style.display='';
		}
		else
		{
			// Absent
			if(((atype == 1039) || (atype == 1178) || (atype == 2877) || (atype == 2879)) && (oSet.half_vacations == 'n'))
			{
				if(id_sel)
				{
					fxf_fn_changeSelectedValue.delay(oFXP.ldelay, id_sel,2571,true);
					id_sel.style.display='none';
				}
				if(id_sel_lit)
					id_sel_lit.innerHTML='';
				if(id_sel_img)
					id_sel_img.style.display='none';
			}
			else
			{
				if(id_sel)
					id_sel.style.display='';
				if(id_sel_lit)
					id_sel_lit.innerHTML='<span class="lit">'+oSet.lit_daytype+':&nbsp;</span>';
				if(id_sel_img)
					id_sel_img.style.display='';
			}

			var abstype=parseInt(fxf_fn_getSelectedValue(id_sel).value);
//alert('abstype='+abstype);
			if(abstype == 2571)
			{
				if(id_end)
					id_end.style.display='';
				if(id_end_lit)
					id_end_lit.innerHTML='<span class="lit">'+oSet.lit_endtime+':&nbsp;</span>';
				if(id_end_img)
					id_end_img.style.display='';
			}
			else
			{
				if(id_end)
					id_end.style.display='none';
				if(id_end_lit)
					id_end_lit.innerHTML='';
				if(id_end_img)
					id_end_img.style.display='none';
			}

			if(id_hrs)
				id_hrs.style.display='none';
		}
	}

	if(achg)
		fxf_fn_waitScreen('');
}

function fxf_fn_adjustAbsentStartDate()
{
	var id_start=fxf_fn_getElement('Soll_Beg_Dtm');
	if(id_start && (id_start.style.display != 'none') && id_start.value.length)
	{
		var sdate=fxf_fn_convertDate2Timestamp(id_start.value);
		id_start.value=sdate.date;

		var id_end=fxf_fn_getElement('Soll_End_Dtm');
		if(id_end && id_end.value.length)
		{
			var edate=fxf_fn_convertDate2Timestamp(id_end.value);
			if(edate.timestamp < sdate.timestamp)
				id_end.value=sdate.date;
			else
				id_end.value=edate.date;
		}
	}
}

function fxf_fn_adjustAbsentEndDate()
{
	var id_end=fxf_fn_getElement('Soll_End_Dtm');
	if(id_end && (id_end.style.display != 'none') && id_end.value.length)
	{
		var edate=fxf_fn_convertDate2Timestamp(id_end.value);
		id_end.value=edate.date;

		var id_start=fxf_fn_getElement('Soll_Beg_Dtm');
		if(id_start && id_start.value.length)
		{
			var sdate=fxf_fn_convertDate2Timestamp(id_start.value);
			if(sdate.timestamp > edate.timestamp)
				id_start.value=edate.date;
			else
				id_start.value=sdate.date;
		}
	}
}

function fxf_fn_checkPID(element)
{
	id_btn=null;
	if(oFXP.action == 3)
		id_btn=$('Button_okay');
	else if(oFXP.action == 4)
		id_btn=$('Button_Loeschen');

	if(id_btn)
	{
		var npid=parseInt(fxf_fn_getSelectedValue(element).value);
		if(npid != tSet.cpid)
			id_btn.disabled=true;
		else
			id_btn.disabled=false;
	}
}

function fxf_fn_changeFP(element)
{
	if(element.checked)
		$('fpparam').style.display='block';
	else
		$('fpparam').style.display='none';
}

function fxf_fn_changeFPScheme(element)
{
	// Get id for selected scheme
	if(element.fxf == 'sl')
		var fmid=parseInt(fxf_fn_getSelectedValue(element).value);
	else
		var fmid=parseInt(element.value);

	var fsign = '';
	var finvc = 0;

	// Get scheme attributes
	var id_sa=$('fp_mod_'+fmid);
	if(id_sa)
	{
		var sa=id_sa.innerHTML.split('|');
		fsign=sa[1];
		finvc=parseInt(sa[2]);
	}
//alert('fxf_fn_changeFPScheme(): fmid='+fmid+' -- fsign='+fsign+', finvc='+finvc);

	// Assign attributes
	var id_fsign=fxf_fn_getElement('ro_fpp_fmkz');
	if(id_fsign)
		id_fsign.innerHTML=fsign;
	id_fsign=fxf_fn_getElement('fpp_fmkz',0,'hidden');
	if(id_fsign)
		id_fsign.value=fsign;

	var id_finvc=fxf_fn_getElement('fpp_abrzyk',0,'select');
	if(id_finvc)
		id_finvc.value=finvc;
}

function fxf_fn_changePrefix(element)
{
	var idpf=fxf_fn_getField($('pprefix'),true);
	if(idpf && !idpf.disabled && (idpf.value.length || tSet.prefix.length))
	{
		var ct=parseInt(fxf_fn_getSelectedValue(element).value);
		new Ajax.Request(fxf_fn_gProgram('prj_prefix', 'contact='+ct+'&oprefix='+fxf_fn_escapeText(tSet.prefix,false)+'&nprefix='+fxf_fn_escapeText(idpf.value,false)+fxf_fn_gParam()),
		{
			method: 'get',

			onSuccess: function(transport)
			{
				var rt=transport.responseText.split(divstr[0]);
//alert('rt='+rt);
				var ec=parseInt(rt[0]);
				if(ec)	// Warning
				{
					fxf_fn_question('', oSet.msg_prefix, ['OK'], [''], 100);
				}
				else
				{
					idpf.value=rt[1];
					tSet.prefix=rt[1];
					if(oFXP.tr != 189)
						fxf_fn_saveFields(idpf.id+'=tx'+fxf_fn_escapeText(idpf.value,false), 'frm='+element.attributes['fxform'].value);
				}
			}
		});
	}

//	scContact();	// Calculate cost popup
}

function fxf_fn_enlarge(enl)
{
	if(enl)
	{
		oID.fxtool.style.display='none';
		oID.fxpath.style.display='none';
		oID.fxtaction.style.display='none';

		gSet.maintop=oID.fxmain.style.top;
		new Effect.Morph(oID.fxmain, {style:'top:-30px', duration: 0.2});

		$('esenl').style.display='none';
		$('esshr').style.display='';
	}
	else
	{
		oID.fxmain.style.top=gSet.maintop;

		oID.fxtool.style.display='';
		oID.fxpath.style.display='';
		oID.fxtaction.style.display='';

		$('esenl').style.display='';
		$('esshr').style.display='none';
	}
}

function fxf_fn_resizeMatrix()
{
	if(oFXP.tr == 189)
	{
		var rcd=$('rcfields_425');
		if(rcd)
		{
			var rcf=rcd.innerHTML.split('|');
			if(rcf && rcf.length)
			{
				var twidth=6;
				var cd='';
				for(var f=0; f<rcf.length; f++)
				{
					cd=$('dw_'+rcf[f]);
					if(cd && (cd.style.display != 'none'))
						twidth += (parseInt(cd.style.width)+1);
				}
//alert('twidth='+twidth);
				oID.dheaderlits.style.width=(twidth+dim.sd.pwidth)+'px';
				oID.dwdata.style.width=twidth+'px';
			}
		}
	}
}

function fxf_fn_getSelPopup(sm,ssm,value)
{
	var html = '<ul>';
	var lines = ssm.innerHTML.split(divstr[1]);
//alert('ssm.id='+ssm.id+'\n\nlines='+lines);
	if(lines.length)
	{
		var md='';
		if(Prototype.BrowserFeatures['Mobile'].length)
			md=' onclick="fxf_eh_mouseDown();"';
		for(var i=0; i<lines.length; i++)
		{
			var data=lines[i].split(divstr[0]);
			var ca='';
			if(value == data[0])
				ca='sel';
			var valid=true;
			var disabled=false;
			var dtt='';
			// Status of tasks with actual effort or invoiced budget cannot be changed back to "planned"
			// Status of tasks with planned but unapproved budget cannot be changed to "active" (as client setting)
			// Status of tasks with open incoming invoices cannot be changed to "finished"
			if((oFXP.tr == 189) && (ssm.id == 'sel_status') && ((data[0] == 297) || (data[0] == 300) || (data[0] == 301)))
			{
				var ei=sm.id;
				var ui='';
				var pc=-1;
				var ib=ei.indexOf('[');
				if(ib >= 0)
				{
					ui=ei.substr(ib+1,ei.length-ib-2);
					ei=ei.substr(0,ib);
					pc=fxf_fn_getKeyFromID('P',ui);
				}
				if((data[0] == 297) && (pc >= 0) && ((pArray[pc].values.aeffort > 0.0) || (pArray[pc].values.ehrinv > 0.0) || (pArray[pc].values.emrinv > 0.0) || (pArray[pc].values.ihrused > 0.0) || (pArray[pc].values.ehrused > 0.0) || (pArray[pc].values.imrused > 0.0) || (pArray[pc].values.emrused > 0.0) || pArray[pc].values.adates.length))
					valid=false;
				else if((data[0] == 300) && (pc >= 0) && (oSet.budappr_auto == 'n') && (oSet.budappr_active == 'j') && (pArray[pc].values.status != 300) && ((!pArray[pc].values.ihrplan && !pArray[pc].values.ihrappr && !pArray[pc].values.imrplan && !pArray[pc].values.imrappr) || (pArray[pc].values.ihrplan && !pArray[pc].values.ihrappr) || (pArray[pc].values.imrplan && !pArray[pc].values.imrappr)))
				{
					disabled=true;
					var bt='';
					if(oFXP.lang == 1)
					{
						if(!pArray[pc].values.ihrplan && !pArray[pc].values.ihrappr && !pArray[pc].values.imrplan && !pArray[pc].values.imrappr)
							dtt='Aktivierung nicht möglich, da weder Personen (HR) noch Material Budget<br />geplant und/oder genehmigt wurde!';
						else
						{
							if(pArray[pc].values.ihrplan && !pArray[pc].values.ihrappr)
								bt='Personen (HR)';
							if(pArray[pc].values.imrplan && !pArray[pc].values.imrappr)
							{
								if(bt.length)
									bt += ' und ';
								bt += 'Material';
							}
							dtt='Aktivierung nicht möglich, da das geplante '+bt+' Budget<br />noch <b>nicht</b> genehmigt wurde!';
						}
					}
					else
					{
						if(!pArray[pc].values.ihrplan && !pArray[pc].values.ihrappr && !pArray[pc].values.imrplan && !pArray[pc].values.imrappr)
							dtt='Activation not possible, because neither the HR nor the MR budget<br />has been planned and /or approved yet!';
						else
						{
							if(pArray[pc].values.ihrplan && !pArray[pc].values.ihrappr)
								bt='HR';
							if(pArray[pc].values.imrplan && !pArray[pc].values.imrappr)
							{
								if(bt.length)
									bt += ' and ';
								bt += 'MR';
							}
							dtt='Activation not possible, because the planned '+bt+' budget<br />has <b>not</b> been approved yet!';
						}
					}
				}
				else if((data[0] == 301) && (pc >= 0))
				{
					// Check open incoming invoices
					var di=pArray[pc].info.split(',');
					var v=parseInt(di[5]);
					if(v)
					{
						disabled=true;
						if(oFXP.lang == 1)
							dtt='Abschluss nicht möglich, da noch <b>offene</b><br />Eingangsrechnungen ausstehen!';
						else
							dtt='Completion not possible, because there are<br />still <b>open</b> incoming invoices outstanding!';
					}
				}
			}
			// Filter person with selected skills
			else if((oFXP.tr == 189) && (ssm.id == 'sel_hr') && (data[0] > 0) && (data.length > 4))
			{
				var ei=sm.id;
				var ui='';
				var pc=-1;
				var ib=ei.indexOf('[');
				if(ib)
				{
					ui=ei.substr(ib+1,ei.length-ib-2);
					ei=ei.substr(0,ib);
					pc=fxf_fn_getKeyFromID('P',ui);
				}
				if((pc >= 0) && (parseInt(pArray[pc].values.skill) > 0))
				{
					ib=data[4].indexOf('('+pArray[pc].values.skill+')');
					if(ib < 0)
						valid=false;
				}
				// Always display already assigned persons
				var hr=pArray[pc].values.hr+'';
				if(!valid && ((hr.substr(0,1) == '*') || (parseInt(hr) > 0)))
				{
					if(hr.substr(0,1) == '*')
						hr=hr.substr(1);
					var ars=hr.split(',');
					for(var a=0; a<ars.length; a++)
					{
						if(ars[a] == data[0])
						{
							valid=true;
							break;
						}
					}
				}
			}
			if(valid)
			{
				var sea=fxf_fn_getSelStyle(data[1]);
				if(disabled || sea.dis)
				{
					if(dtt.length)
						dtt=' tooltip="<font class=red>'+dtt+'</font>" style="cursor:help;"';
					html += '<li id="fspo-'+(i+1)+'" class="sdisabled"'+dtt+' style="'+sea.st+'"'+md+'>'+sea.tx+'</li>';
				}
				else
				{
					dtt='';
					if(sea.tt.length)
					{
						dtt=' tooltip=\''+sea.tt+'\'';
						sea.st += 'cursor:help;';
					}
					html += '<li id="fspo-'+(i+1)+'" class="sentry'+ca+'" svalue="'+data[0]+'"'+dtt+' style="'+sea.st+'"'+md+'>'+sea.tx+'</li>';
				}
			}
//alert(i+': data='+data);
		}
	}
	html += '</ul>';
//alert('html:\n\n'+html);

	return html;
}

function fxf_fn_getSelStyle(t)
{
	var r={'st':"", 'bg':"", 'tx':t, 'tt':"", 'dis':false};

	if((t !== undefined) && t.length)
	{
		r.tx = r.tx.replace(/\&lt\;\/d\&gt\;/g, '').replace(/\<\/d\>/g, '');

		var i=r.tx.indexOf('&lt;d&gt;');
		if(i >= 0)
		{
			r.dis = true;
			r.tx = r.tx.replace(/\&lt\;d\&gt\;/g, '');
		}
		else
		{
			i=r.tx.indexOf('<d>');
			if(i >= 0)
			{
				r.dis = true;
				r.tx = r.tx.replace(/\<d\>/g, '');
			}
		}

		i=r.tx.indexOf('&lt;b&gt;');
		if(i >= 0)
		{
			r.st += 'font-weight:bold;';
			r.tx = r.tx.replace(/\&lt\;b\&gt\;/g, '').replace(/\&lt\;\/b\&gt\;/g, '');
		}
		else
		{
			i=r.tx.indexOf('<b>');
			if(i >= 0)
			{
				r.st += 'font-weight:bold;';
				r.tx = r.tx.replace(/\<b\>/g, '').replace(/\<\/b\>/g, '');
			}
		}

		i=r.tx.indexOf('&lt;u&gt;');
		if(i >= 0)
		{
			r.st += 'text-decoration:underline;';
			r.tx = r.tx.replace(/\&lt\;u\&gt\;/g, '').replace(/\&lt\;\/u\&gt\;/g, '');
		}
		else
		{
			i=r.tx.indexOf('<u>');
			if(i >= 0)
			{
				r.st += 'text-decoration:underline;';
				r.tx = r.tx.replace(/\<u\>/g, '').replace(/\<\/u\>/g, '');
			}
		}

		i=r.tx.indexOf('&lt;i&gt;');
		if(i >= 0)
		{
			r.st += 'font-style:italic;color:#444444;';
			r.tx = r.tx.replace(/\&lt\;i\&gt\;/g, '').replace(/\&lt\;\/i\&gt\;/g, '');
		}
		else
		{
			i=r.tx.indexOf('<i>');
			if(i >= 0)
			{
				r.st += 'font-style:italic;color:#444444;';
				r.tx = r.tx.replace(/\<i\>/g, '').replace(/\<\/i\>/g, '');
			}
		}

		i=r.tx.indexOf('&lt;bb&gt;');
		if(i >= 0)
		{
			r.st += 'border-bottom:1px solid #888888;';
			r.tx = r.tx.replace(/\&lt\;bb\&gt\;/g, '');
		}
		else
		{
			i=r.tx.indexOf('<bb>');
			if(i >= 0)
			{
				r.st += 'border-bottom:1px solid #888888;';
				r.tx = r.tx.replace(/\<bb\>/g, '');
			}
		}

		i=r.tx.indexOf('&lt;bg#');
		if(i >= 0)
		{
			r.bg = '#'+r.tx.substr(i+7,6);
			r.st += 'background:'+r.bg+';';
			r.tx = r.tx.replace(/\&lt\;bg\#......\&gt\;/g, '');
		}
		else
		{
			i=r.tx.indexOf('<bg#');
			if(i >= 0)
			{
				r.bg = '#'+r.tx.substr(i+4,6);
				r.st += 'background:'+r.bg+';';
				r.tx = r.tx.replace(/\<bg\#......\>/g, '');
			}
		}

		i=r.tx.indexOf('&lt;img');
		if(i >= 0)
			r.tx = r.tx.replace(/\&lt\;img/g, '<img').replace(/\/\&gt\;/g, '/>');

		i=r.tx.indexOf('&lt;tt&gt;');
		if(i >= 0)
		{
			var ie = r.tx.indexOf('&lt;/tt&gt;', i+10);
			r.tt = r.tx.substring(i+10, ie);
			r.tx = r.tx.replace(/\&lt\;tt\&gt\;.*\&lt\;\/tt\&gt\;/g, '');
		}
		else
		{
			i=r.tx.indexOf('<tt>');
			if(i >= 0)
			{
				var ie = r.tx.indexOf('</tt>', i+4);
				r.tt = r.tx.substring(i+4, ie);
				r.tx = r.tx.replace(/\<tt\>.*\<\/tt\>/g, '');
			}
		}

		i=r.tx.indexOf('&lt;blue&gt;');
		if(i >= 0)
		{
			r.st += 'color:#006b9f;';
			r.tx = r.tx.replace(/\&lt\;blue\&gt\;/g, '').replace(/\&lt\;\/blue\&gt\;/g, '');
		}
		else
		{
			i=r.tx.indexOf('<blue>');
			if(i >= 0)
			{
				r.st += 'color:#006b9f;';
				r.tx = r.tx.replace(/\<blue\>/g, '').replace(/<\/blue\>/g, '');
			}
			else
			{
				i=r.tx.indexOf('&lt;red&gt;');
				if(i >= 0)
				{
					r.st += 'color:#e1001a;';
					r.tx = r.tx.replace(/\&lt\;red\&gt\;/g, '').replace(/\&lt\;\/red\&gt\;/g, '');
				}
				else
				{
					i=r.tx.indexOf('<red>');
					if(i >= 0)
					{
						r.st += 'color:#e1001a;';
						r.tx = r.tx.replace(/\<red\>/g, '').replace(/<\/red\>/g, '');
					}
					else
					{
						i=r.tx.indexOf('&lt;green&gt;');
						if(i >= 0)
						{
							r.st += 'color:#009f6b;';
							r.tx = r.tx.replace(/\&lt\;green\&gt\;/g, '').replace(/\&lt\;\/green\&gt\;/g, '');
						}
						else
						{
							i=r.tx.indexOf('<green>');
							if(i >= 0)
							{
								r.st += 'color:#009f6b;';
								r.tx = r.tx.replace(/\<green\>/g, '').replace(/<\/green\>/g, '');
							}
							else
							{
								i=r.tx.indexOf('&lt;grey&gt;');
								if(i >= 0)
								{
									r.st += 'color:#777777;';
									r.tx = r.tx.replace(/\&lt\;grey\&gt\;/g, '').replace(/\&lt\;\/grey\&gt\;/g, '');
								}
								else
								{
									i=r.tx.indexOf('<grey>');
									if(i >= 0)
									{
										r.st += 'color:#777777;';
										r.tx = r.tx.replace(/\<grey\>/g, '').replace(/<\/grey\>/g, '');
									}
								}
							}
						}
					}
				}
			}
		}

		if(r.dis && !r.tx.length)
			r.st += 'height:2px;';

			if(oFXP.lang == 1)
			i=r.tx.indexOf('{inaktiv}');
		else
			i=r.tx.indexOf('{inactive}');
		if(i >= 0)
			r.st += 'font-style:italic;color:#888888;';

		if(oFXP.lang == 1)
			i=r.tx.indexOf('{gesperrt}');
		else
			i=r.tx.indexOf('{locked}');
		if(i >= 0)
			r.st += 'font-style:italic;color:#e1001a;';

		var l3=r.tx.substr(r.tx.length-3,3);
		if(l3 == '**}')
			r.st += 'font-style:italic;font-weight:bold;';
		else if(l3 == '{*}')
			r.st += 'font-style:italic;color:#333333;';
	}

//alert('r: st='+r.st+', bg='+r.bg+', tx='+r.tx+', tt='+r.tt+', dis='+r.dis);
	return r;
}

function fxf_fn_drawSelPopup(sm,html)
{
	var data=html.split('*~*');
//alert('fxf_fn_drawSelPopup('+sm.uid+', '+sm.attributes['svalue'].value+')\n\ndata.length='+data.length+'\n\n'+data);
	if(data.length > 0)
	{
		oID.selpop.innerHTML=fxf_fn_escapeText(data[0],true);

		sm.className='focus'+sm.className;

		oVar.lastSelUID=sm.uid;
		oVar.lastSelElement=sm;

		var cr=fxf_fn_getBCR(sm);
		var sp=22;
		var spo=0;
		var ispst=$('selpopptype_'+oVar.lastSelUID);
		var cpst=false;
		if(ispst && (oFXP.tr != 268))
		{
			spo=20;
			var pst=ispst.value.split('|');
			// Main project
			var cb=$('selpopptypecbmp');
			cb.checked=false;
			var dv=$('selpopptypedivmp');
			if(pst[0] != '-')
			{
				dv.style.display='';
				if(pst[0] == '1')
					cb.checked=true;
				else
					cpst=true;
			}
			else
				dv.style.display='none';
			// Sub project
			cb=$('selpopptypecbsp');
			cb.checked=false;
			dv=$('selpopptypedivsp');
			if(pst[1] != '-')
			{
				dv.style.display='';
				if(pst[1] == '1')
					cb.checked=true;
				else
					cpst=true;
			}
			else
				dv.style.display='none';
			// Task
			cb=$('selpopptypecbts');
			cb.checked=false;
			dv=$('selpopptypedivts');
			if(pst[2] != '-')
			{
				dv.style.display='';
				if(pst[2] == '1')
					cb.checked=true;
				else
					cpst=true;
			}
			else
				dv.style.display='none';
		}

		// Get select area height
		if((sm.className == 'focusfxfslm') || (sm.className == 'focusfmfslm'))
			oID.selpop.className='selcolm';
		else
			oID.selpop.className='selcol';
		oID.selpop.style.width='auto';
		oID.selpop.style.height='auto';
		oID.selpop.style.display='';
		var soh=oID.selpop.clientHeight;
		var sh=soh;

		// Place select area at bottom of select field?
		var pb=1;
		var st=cr.bottom-2+spo;
		if((dim.sd.pheight-cr.bottom-sp-spo < sh) && (cr.top-sp-spo > dim.sd.pheight-cr.bottom-sp-spo))
		{
			pb=0;
			st=cr.top-2-spo;
		}

		// Adjust select area height
		if(pb)
			sh=Math.min(sh, dim.sd.pheight-cr.bottom-sp-spo);
		else
		{
			sh=Math.min(sh, cr.top-sp-spo);
			st -= sh;
		}

		// Add scrollbar width to select area width?
		var as=0;
		if(sh < oID.selpop.clientHeight)
			as=18;

		// Get select area width
		var sw=Math.max(cr.right-cr.left-2, oID.selpop.clientWidth+as);

		// Place select area left aligned of select field?
		var pl=1;
		var sl=cr.left;
		if((dim.sd.pwidth-cr.left-sp < sw) && (cr.right-sp > dim.sd.pwidth-cr.left-sp))
		{
			pl=0;
			sl=cr.right-1;
		}

		// Adjust select area width
		if(pl)
			sw=Math.min(sw, dim.sd.pwidth-cr.left-sp);
		else
		{
			sw=Math.min(sw, cr.right-sp);
			sl -= sw;
		}

		// Position select area
//alert('dim.sd.pwidth='+dim.sd.pwidth+', dim.sd.pheight='+dim.sd.pheight+'\ncr.left='+cr.left+', cr.right='+cr.right+', cr.top='+cr.top+', cr.bottom='+cr.bottom+'\noID.selpop.clientWidth='+oID.selpop.clientWidth+', oID.selpop.clientHieght='+oID.selpop.clientHeight+'\n\nsl='+sl+', st='+st+' - sw='+sw+', sh='+sh);
		oID.selpop.style.left=sl+'px';
		oID.selpop.style.top=st+'px';
		oID.selpop.style.width=sw+'px';
		oID.selpop.style.height=sh+'px';

		// Position project type selection
		if(spo)
		{
			if(pb)
				oID.selpopptype.style.top=(st-spo)+'px';
			else
				oID.selpopptype.style.top=(st+sh)+'px';
			oID.selpopptype.style.left=sl+'px';
			oID.selpopptype.style.width=sw+'px';
			oID.selpopptype.style.display='';
		}

		// Position select search
		selsc=0;
		selentries=[];
		var stwidth=cr.width-28;
		if(stwidth > 60)
		{
			var sels=$$('[id^="fspo-"]');
			var entrysel=$$('[class="sentrysel"]');
			var ep=0;
			if(sels && sels.length)
			{
				for(var s=0; s<sels.length; s++)
				{
					sels[s].style.display='';
					selentries[s]=sels[s].innerHTML;
					if(entrysel.length && (entrysel[0].id == sels[s].id))
						ep=s;
				}
				var stheight=cr.height-6;
				var stext=sm.innerHTML;
				var mand=sm.className.substr(sm.className.length-1,1);
				if(mand != 'm')
					mand='';
				var sdio=stext.indexOf('<div');
				if(sdio > 0)
					stext=stext.substr(0,sdio);
				else
				{
					var sdio=stext.indexOf('<DIV');
					if(sdio > 0)
						stext=stext.substr(0,sdio);
				}
				oID.selpopsearchtext.value=fxf_fn_convertHTMLEntities(stext,false);
				oID.selpopsearchtext.style.width=(stwidth-25)+'px';
				oID.selpopsearchtext.style.top=Math.min(4,(stheight-14))+'px';
				oID.selpopsearch.style.left=cr.left+'px';
				oID.selpopsearch.style.top=(cr.top-1)+'px';
				oID.selpopsearch.style.width=stwidth+'px';
				oID.selpopsearch.style.height=(cr.height-10)+'px';
				oID.selpopsearch.className='focuscls'+mand;
				oID.selpopsearch.style.display='';
			}

			// Scroll to selected entry
			var lh=16;
			if(entrysel.length)
			{
				var so=fxf_fn_getBCR(entrysel[0]);
				lh=so.bottom-so.top;
			}
			else if(Prototype.Browser.IE)
				lh=17;
			oID.selpop.scrollTop=ep*lh;
		}

		oID.selpop.focus();

		if(cpst)
			fxf_fn_searchSelPopup(0);
	}
}

function fxf_fn_drawChangedSelectedValue(element,svalue,txt,save)
{
	var cn3=fxf_fn_getSelType(element);
	var rt=txt.split('|');
	var sv=svalue;
	if(rt.length > 1)
		sv=rt[1];
	var sea=fxf_fn_getSelStyle(rt[0]);
	element.innerHTML=sea.tx+'<div class="'+cn3+'slc" style="cursor:pointer;"><span class="'+cn3+'sli"></span></div>';
	element.attributes['svalue'].value=sv;
	element.fxv=sv;
	element.style.background=sea.bg;
	if(save)
		fxf_fn_ajaxSaveElement.delay(oFXP.ldelay, element);

	if(element.id.substr(0,6) == 'rcsel_')
	{
		var mss=sv.split('_');
//alert('mss='+mss);
		var mn=element.id.substr(6);
		var eelement=fxf_fn_getField($('rctxt_'+mn),true);
		var rcres=$('rcres_'+mn);
		var rcsav=$('rcsav_'+mn);
		var rcdel=$('rcdel_'+mn);

		// Edit
		if(mss[0] == 'x')
		{
			element.style.display='none';
			eelement.style.display='';
			rcres.setOpacity(0.95);
			rcsav.setOpacity(0.95);
			rcdel.setOpacity(0.35);
		}
		// Select
		else
		{
			element.style.display='';
			eelement.style.display='none';
			rcres.setOpacity(0.35);
			rcsav.setOpacity(0.35);
			rcdel.setOpacity(0.35);
			if(mss[0].length > 1)
			{
				var msp=mss[0].split('-');
				var pid=parseInt(msp[1]);
				if(oSet.person == pid)
					rcdel.setOpacity(0.95);
			}
		}
	}
}

function fxf_fn_ppspDelta(pc)
{
	var dv={'pp':0.0, 'sp':0.0};

	if(pc >= 0)
	{
		var pe=pArray[pc].values.pe+'';
		if(pe.indexOf(',') > 0)
		{
			var pes=pArray[pc].values.pe.split(',');
			var pcs=pArray[pc].values.pc.split(',');
			var pps=pArray[pc].values.pp.split(',');
			var sps=pArray[pc].values.sp.split(',');
			var efh=0.0;
			var eft=0.0;
			for(var s=0; s<pes.length; s++)
			{
				if(pArray[pc].values.peffort > 0)
					efh=Math.min(parseFloat(pArray[pc].values.peffort), Math.max(0.0, parseFloat(pes[s])));
				else
					efh=Math.min(100.0, Math.max(0.0, parseFloat(pcs[s])*100.0));
				dv.pp += efh*parseFloat(pps[s]);
				dv.sp += efh*parseFloat(sps[s]);
				eft += efh;
			}
			if(eft > 0.0)
			{
				dv.pp=Math.round(dv.pp*100.0/eft)/100.0;
				dv.sp=Math.round(dv.sp*100.0/eft)/100.0;
			}
		}
		else
		{
			dv.pp=pArray[pc].values.pp;
			dv.sp=pArray[pc].values.sp;
		}
	}
	else
	{
		var el=$('delta_pp');
		if(el)
			dv.pp=parseFloat(el.innerHTML);
		else
		{
			el=$('EK');
			if(el)
				dv.pp=fxf_fn_string2float(el.value);
		}

		el=$('delta_sp');
		if(el)
			dv.sp=parseFloat(el.innerHTML);
		else
		{
			el=$('VK');
			if(el)
				dv.sp=fxf_fn_string2float(el.value);
		}
	}

	return dv;
}

function fxf_fn_ppspClose(cls)
{
	var pc=-1;
	var dpc=$('ppsppc');
	if(dpc)
		pc=parseInt(dpc.value);
//alert('ppspClose: pc='+pc);

	var element=null;

	// ...Plan. Effort
	var nef=-1;
	var rbe=$$('[name="ppspnef"]');
	if(rbe.length)
	{
		for(var r=0; r<rbe.length; r++)
		{
			if(rbe[r].checked)
			{
				nef=parseInt(rbe[r].value);
				break;
			}
		}
	}

	// ...Int. Plan. HRB
	var nib=-1.0;
	if(((oSet.cost_see == 1) || (oSet.cost_see == 3)) && ((oSet.cost_btype == 'I') || (oSet.cost_btype == 'B')))
	{
		rbe=$$('[name="ppspnib"]');
		if(rbe.length)
		{
			for(var r=0; r<rbe.length; r++)
			{
				if(rbe[r].checked)
				{
					nib=parseFloat(rbe[r].value);
					break;
				}
			}
		}
	}

	// ...Ext. Plan. HRB
	var neb=-1.0;
	if(((oSet.cost_see == 2) || (oSet.cost_see == 3)) && ((oSet.cost_btype == 'E') || (oSet.cost_btype == 'B')))
	{
		rbe=$$('[name="ppspneb"]');
		if(rbe.length)
		{
			for(var r=0; r<rbe.length; r++)
			{
				if(rbe[r].checked)
				{
					neb=parseFloat(rbe[r].value);
					break;
				}
			}
		}
	}
//alert('Plan. Effort: nef='+nef+'\nInt. Plan. HRB: nib='+nib+'\nExt. Plan. HRB: neb='+neb);

	// Matrix
	if(pc >= 0)
	{
//alert('Plan. Effort: ov='+pArray[pc].values.peffort+', nef='+nef);
		if((nef >= 0) && (nef != pArray[pc].values.peffort))
		{
			element=fxf_fn_getField($('peffort['+pArray[pc].uid+']'),true);
			element.fxv=fxf_fn_sec2unit(nef,-1);
			fxf_fn_adjustColumnEffort(element);
		}

		// ...Int. Plan. HRB
//alert('Int. Plan. HRB: ov='+pArray[pc].values.ihrplan+', nib='+nib);
		if((nib >= 0.0) && (nib != pArray[pc].values.ihrplan))
		{
			element=fxf_fn_getField($('ihrplan['+pArray[pc].uid+']'),true);
			element.fxv=fxf_fn_float2string(nib);
			fxf_fn_adjustColumnBudgetPlan(element,false);
		}

		// ...Ext. Plan. HRB
//alert('Ext. Plan. HRB: ov='+pArray[pc].values.ehrplan+', neb='+neb);
		if((neb >= 0.0) && (neb != pArray[pc].values.ehrplan))
		{
			element=fxf_fn_getField($('ehrplan['+pArray[pc].uid+']'),true);
			element.fxv=fxf_fn_float2string(neb);
			fxf_fn_adjustColumnBudgetPlan(element,false);
		}

		pcc=true;
		fxf_fn_saveProject('*',false);
	}
	else
	{
		var fs='';
		var form='';

		var efu=-1;
		var oef=0;
		element=fxf_fn_getField($('Aufwand_Soll'),true);
		if(element)
		{
			if(!form.length)
				form=element.attributes['fxform'].value;
			efu=fxf_fn_getSelectedValue($('Zeiteinheit_Aufw_S')).value;
			oef=fxf_fn_unit2sec(fxf_fn_float2string(Math.max(0.0, fxf_fn_string2float(element.fxv))),efu);
		}
//alert('Plan. Effort: oef='+oef+', nef='+nef);
		if((nef >= 0) && (efu >= 0) && (nef != oef))
		{
			element.value=fxf_fn_sec2unit(nef,efu);
			element.fxv=element.value;
			if(fs.length)
				fs += '&';
			fs += 'Aufwand_Soll=tx'+element.fxv;
		}
		if(nef >= 0)
			sunit[element.id]=nef;

		// ...Int. Plan. HRB
		var oib=0.0;
		element=fxf_fn_getField($('budget_pers_int_gepl'),true);
		if(element)
		{
			if(!form.length)
				form=element.attributes['fxform'].value;
			oib=fxf_fn_string2float(element.fxv);
		}
//alert('Int. Plan. HRB: oib='+oib+', nib='+nib);
		if((nib >= 0.0) && (nib != oib))
		{
			element.value=fxf_fn_float2string(nib);
			element.fxv=element.value;
			if(fs.length)
				fs += '&';
			fs += 'budget_pers_int_gepl=tx'+element.fxv;
		}

		// ...Ext. Plan. HRB
		var oeb=0.0;
		element=fxf_fn_getField($('budget_pers_ext_gepl'),true);
		if(element)
		{
			if(!form.length)
				form=element.attributes['fxform'].value;
			oeb=fxf_fn_string2float(element.fxv);
		}
//alert('Ext. Plan. HRB: oeb='+oeb+', neb='+neb);
		if((neb >= 0.0) && (neb != oeb))
		{
			element.value=fxf_fn_float2string(neb);
			element.fxv=element.value;
			if(fs.length)
				fs += '&';
			fs += 'budget_pers_ext_gepl=tx'+element.fxv;
		}

		if(fs.length)
			fxf_fn_saveFields(fs, 'frm='+form);
	}

	if(cls)
		fxf_fn_fxLinkClose();
}

function fxf_fn_ppsp_param()
{
	var efu=-1;
	var ppsp_ef=0;
	var element=fxf_fn_getField($('Aufwand_Soll'),true);
	if(element)
	{
		efu=fxf_fn_getSelectedValue($('Zeiteinheit_Aufw_S')).value;
		ppsp_ef=fxf_fn_unit2sec(fxf_fn_float2string(Math.max(0.0, fxf_fn_string2float(element.fxv))),efu);
	}
	var ppsp=fxf_fn_ppspDelta(-1);
	var ppsp_pr=Math.max(0,parseInt($('Projekt_ID').value));
	var ppsp_no=$('Vorgangsnummer').value+$('Vorgang_naechster').value;
	var ppsp_nm=$('Name_Projekt').value;
	var ppsp_cp=parseInt(fxf_fn_getSelectedValue($('Ansprechpartner')).value);
	var ppsp_sk=fxf_fn_getSelectedValue($('MaArt_ID')).value;
	element=$('delta_hr');
	if(element)
	{
		var hrs=element.innerHTML.split('|');
//alert('hrs='+hrs);
		var ppsp_hr=hrs[0];
		var ppsp_pc=hrs[1];
		var ppsp_pe=hrs[2];
		var ppsp_pp=hrs[3];
		var ppsp_sp=hrs[4];
	}
	else
	{
		var ppsp_hr=fxf_fn_getSelectedValue($('Personen_ID')).value;
		var ppsp_pc=1.0;
		var ppsp_pe=ppsp_ef;
		var ppsp_pp=ppsp.pp;
		var ppsp_sp=ppsp.sp;
	}
	var ppsp_ib=0.0;
	if((oSet.cost_see != 2) && (oSet.cost_btype != 'E'))
		ppsp_ib=fxf_fn_string2float($('budget_pers_int_gepl').value);
	var ppsp_eb=0.0;
	if((oSet.cost_see != 1) && (oSet.cost_btype != 'I'))
		ppsp_eb=fxf_fn_string2float($('budget_pers_ext_gepl').value);
//alert('ID='+ppsp_pr+', No='+ppsp_no+', Name='+ppsp_nm+', Contact='+ppsp_cp+', Skill='+ppsp_sk+', HR='+ppsp_hr+', PC='+ppsp_pc+', PE='+ppsp_pe+', PP='+ppsp.pp+', SP='+ppsp.sp+', Effort='+ppsp_ef+', IPB='+ppsp_ib+', EPB='+ppsp_eb);

	return 'pr='+ppsp_pr+'&no='+ppsp_no+' '+fxf_fn_escapeText(ppsp_nm,false)+'&cp='+ppsp_cp+'&sk='+ppsp_sk+'&hr='+ppsp_hr+'&pc='+ppsp_pc+'&pe='+ppsp_pe+'&pp='+ppsp_pp+'&sp='+ppsp_sp+'&ef='+ppsp_ef+'&ib='+ppsp_ib+'&eb='+ppsp_eb;
}

function fxf_fn_convertVal(val,ftype,dtype)
{
	// Everything except Selectfield
	if(dtype != 8)
	{
		// Date
		if(ftype == 7)
			val=fxf_fn_convertDate2Timestamp(val).timestamp;
		// Yes/No
		else if(ftype == 4)
		{
			if(val)
				val='1';
			else
				val='';
		}
		// Integer
		else if((ftype == 5) || (ftype == 12))
		{
			val=parseInt(val);
			if(isNaN(val))
				val=0;
		}
		// Float (Decimal, Budgets, Costs or Percentages)
		else if((ftype == 6) || (ftype == 16) || (ftype == 17) || (ftype == 51) || (ftype == 52))
			val=Math.max(0.0, fxf_fn_string2float(val));
		// Duration
		else if(ftype == 50)
			val=fxf_fn_unit2sec(fxf_fn_float2string(Math.max(0.0, fxf_fn_string2float(val))),-1);
	}

	return val;
}

function fxf_fn_changePType()
{
	var id_ptype=fxf_fn_getField($('Projektart'),true);
	var id_parent=fxf_fn_getField($('Elter'),true);
	if(id_ptype && id_parent)
	{
		var id_tr=$('td_lit_Elter').up();
		var val_ptype=parseInt(fxf_fn_getSelectedValue(id_ptype).value);
		var val_parent=parseInt(fxf_fn_getSelectedValue(id_parent).value);
//alert('id_ptype='+id_ptype+', val_ptype='+val_ptype+'\nid_parent='+id_parent+', val_parent='+val_parent);
		if(val_ptype < 60)
		{
			if(val_ptype == 59)
				tSet.pval=val_parent;
			if(id_tr.style.display != 'none')
			{
				fxf_fn_changeSelectedValue(id_parent,0,true);
				id_tr.style.display='none';
			}
		}
		else
		{
			if(id_tr.style.display == 'none')
				id_tr.style.display='';
			if((tSet.pval > 0) && (val_parent == 0))
				fxf_fn_changeSelectedValue(id_parent,tSet.pval,true);
		}
	}
}

function fxf_fn_reloadApp(id,url,param,event)
{
	var div=$(id);
	if(div)
	{
		fxf_fn_waitScreen('reloading');
		url += '?__ctr='+oFXP.tr;
		if(param.length)
			url += '&'+param;
		url += fxf_fn_gParam();
//alert('url:\n'+url);
		new Ajax.Request(url,
		{
			method:'get', asynchronous:true,
			onSuccess: function(transport)
			{
				var ret=transport.responseText;
//alert('ret:\n'+ret);
				div.innerHTML=ret;
				fxf_fn_waitScreen('');
			},
			onFailure: function()
			{
				fxf_fn_waitScreen('');
			}
		});
	}
}

function fxf_fn_ccReloadInv(mc, event)
{
	var id_ccinv=$('ccinv');
	var id_past_months=fxf_fn_getField($('past_months'),true);
	if(id_ccinv && id_past_months)
	{
		if(id_past_months.value.length)
			var m=Math.max(0,parseInt(id_past_months.value));
		else
			var m='';
		id_past_months.value=m;
		var url=fxf_fn_gProgram('oinvoices', 'm='+m+'&mc='+mc+fxf_fn_gParam());
//alert('url='+url);
		new Ajax.Request(url,
		{
			method:'get', asynchronous:true,
			onSuccess: function(transport)
			{
				var ret=transport.responseText;
				id_ccinv.innerHTML=ret;
				fxf_fn_waitScreen('');
				if(oFXP.tr == 14)
					fxf_fn_init14();
			}
		});
	}
}

function fxf_fn_markText(elid)
{
	if(document.selection)
	{
		var range=document.body.createTextRange();
		range.moveToElementText(document.getElementById(elid));
		range.select();
	}
	else if(window.getSelection)
	{
		var range=document.createRange();
		range.selectNode(document.getElementById(elid));
		window.getSelection().addRange(range);
	}
}

function fxf_fn_cursorText(element)
{
	if(element.setSelectionRange)
		element.setSelectionRange(element.value.length, element.value.length);
	else if(element.createTextRange)
	{
		var range=element.createTextRange();
		range.collapse(true);
		range.moveStart('character', element.value.length);
		range.moveEnd('character', element.value.length);
		range.select();
	}
}

function fxf_fn_searchSelPopup(kc)
{
	var entrysel=null;
	var dec=0;

	var les='';
	var upst=false;
	var nomp=false;
	var nosp=false;
	var nots=false;
	var pstv='';
	var ispst=$('selpopptype_'+oVar.lastSelUID);
	if(ispst)
	{
		var pst=ispst.value.split('|');
		// Main project
		if(pst[0] != '-')
		{
			var cb=$('selpopptypecbmp');
			if(cb && !cb.checked)
			{
				nomp=true;
				upst=true;
				pstv += '0';
			}
			else
				pstv += '1';
		}
		else
		{
			$nomp=true;
			pstv += '-';
		}
		// Sub project
		pstv += '|';
		if(pst[1] != '-')
		{
			var cb=$('selpopptypecbsp');
			if(cb && !cb.checked)
			{
				nosp=true;
				upst=true;
				pstv += '0';
			}
			else
				pstv += '1';
		}
		else
		{
			nosp=true;
			pstv += '-';
		}
		// Task
		pstv += '|';
		if(pst[2] != '-')
		{
			var cb=$('selpopptypecbts');
			if(cb && !cb.checked)
			{
				nots=true;
				upst=true;
				pstv += '0';
			}
			else
				pstv += '1';
		}
		else
		{
			nots=true;
			pstv += '-';
		}

		ispst.value=pstv;
//alert('oVar.lastSelUID='+oVar.lastSelUID+', upst='+upst+', nomp='+nomp+', nosp='+nosp+', nots='+nots+' -- pstv='+pstv);
	}
//var dsplst='';

	var schars=oSet.sel_max_c;
	var slines=oSet.sel_max_l;

	// Single select
	if(oID.selpoptoggle.style.display == 'none')
	{
		var sels=null;
		var dec=$('sec_'+oVar.lastSelUID);
		var sec=0;
		if(dec)
			sec=parseInt(dec.innerHTML);
		else
		{
			var sels=$$('[id^="fspo-"]');
			sec=sels.length;
		}
		if(oVar.selsearch)
			var search=oID.selpopsearchtext.value.replace(/\ /g, '¤').replace(/\;/g, '¡').replace(/\&/g, '&amp;').replace(/\</g, '&lt;').replace(/\>/g, '&gt;').toLowerCase();
		else
			var search='';
		var sl=search.length;
		var flt=0;
		if(sec && (!sl || (kc == 1969) || (!schars && !slines) || ((schars >= 0) && (slines >= 0) && ((sec <= slines) || (schars && (sl >= schars))))))
			flt=1;
		if(flt)
		{
			if(!sels)
				sels=$$('[id^="fspo-"]');
			if(!sels || !selentries || !sels.length || (selentries.length != sels.length))
				flt=0;
			else if(!sec)
				sec=sels.length;
		}
//fxf_fn_taskMsg('Single select: schars='+schars+', slines='+slines+', key='+kc+' -- sec='+sec+', search='+search+' (length='+search.length+') -- flt='+flt);
		if(flt)
		{
			var fdl=-1;
			var lel=-1;
			for(var s=0; s<sels.length; s++)
			{
				if(sels[s].className == 'sentrysel')
					entrysel=sels[s];
				sels[s].innerHTML=selentries[s];
				var id='';
				if(sels[s].attributes && sels[s].attributes['svalue'])
					id=sels[s].attributes['svalue'].value;
				var value=selentries[s];
				if(id.length && (id != '0') && value.length && search.length)
				{
					var ir='';
					var ii=value.indexOf('<img');
					if(ii >= 0)
					{
						var ie=value.indexOf('>', ii+4);
						var ir=value.substring(ii,ie+1);
						value = value.replace(/\<img.*\>/g, '¨');
					}
					value=value.replace(/\ /g, '¤').replace(/\&nbsp\;/g, '¤');
					var pf=value.toLowerCase().indexOf(search);
					if((pf < 0) && (sels[s].className == 'sentry'))
						sels[s].style.display='none';
					else
					{
						if(!entrysel)
							dec++;
						if(pf >= 0)
						{
							while(pf >= 0)
							{
								var ins='<font id="fsps-'+sels[s].id.substr(5)+'" class="red">'+value.substr(pf,search.length)+'</font>';
								value=value.substr(0,pf)+ins+value.substr(pf+search.length);
								pf=value.toLowerCase().indexOf(search,pf+ins.length);
							}
							value=value.replace(/\¤/g, '&nbsp;').replace(/\¨/g, ir);
							sels[s].innerHTML=value;
						}
						sels[s].style.display='';
					}
				}
				else
				{
					sels[s].style.display='';
					if(!entrysel)
						dec++;
				}

				// Filter project type?
				if(id.length && (id != '0') && value.length && upst && (nomp || nosp || nots) && (sels[s].className != 'sentrysel') && (sels[s].style.display != 'none'))
				{
					les=selentries[s].substr(selentries[s].length-3,3);
					if(les == '**}')
					{
						if(nomp)
							sels[s].style.display='none';
					}
					else if(les == '{*}')
					{
						if(nosp)
							sels[s].style.display='none';
					}
					else if(nots)
						sels[s].style.display='none';
				}
//dsplst += s+': id='+id+', innerHTML='+sels[s].innerHTML+', display='+sels[s].style.display+'\n';

				if((!id.length || !value.length) && (upst && nosp && nots))
					sels[s].style.display='none';
				else if(search.length || (upst && (nomp || nosp || nots)))
				{
					if(!id.length || !selentries[s].length)
					{
						sels[s].style.display='none';
						if(!selentries[s].length)
							lel=s;
					}
					else if((sels[s].style.display != 'none') && (lel >= 0))
					{
						if(fdl < 0)
							fdl=lel;
						for(var h=lel; h<s; h++)
						{
							if((h != fdl) && (!sels[h].attributes || !sels[h].attributes['svalue'] || !selentries[h].length))
								sels[h].style.display='';
						}
						lel=-1;
					}
				}
			}
		}
	}

	// Multi select
	else if(mselinfo.entries && mselinfo.entries.length && selentries && selentries.length && (selentries.length == mselinfo.entries.length))
	{
		var sec=mselinfo.entries.length;
		if(oVar.selsearch)
			var search=oID.selpopsearchtext.value.replace(/\ /g, '¤').replace(/\;/g, '¡').replace(/\&/g, '&amp;').replace(/\</g, '&lt;').replace(/\>/g, '&gt;').toLowerCase();
		else
			var search='';
		var sl=search.length;
		var flt=0;
		if(sec && (!sl || (kc == 1969) || (!schars && !slines) || ((schars >= 0) && (slines >= 0) && ((sec <= slines) || (schars && (sl >= schars))))))
			flt=1;
//fxf_fn_taskMsg('Multi select: schars='+schars+', slines='+slines+', key='+kc+' -- sec='+sec+', search='+search+' (length='+search.length+') -- flt='+flt);
		if(flt)
		{
			var fdl=-1;
			var lel=-1;
			for(var s=0; s<mselinfo.entries.length; s++)
			{
				if(mselinfo.entries[s].className == 'msentrysel')
					entrysel=mselinfo.entries[s];
				var id='';
				if(mselinfo.entries[s].attributes && mselinfo.entries[s].attributes['svalue'])
					id=mselinfo.entries[s].attributes['svalue'].value;
				var disp=false;
				var value=mselinfo.entries[s].innerHTML;
				if(id.length && (id != '0') && value.length && search.length)
				{
					var value=value.replace(/\ /g, '¤').replace(/\&nbsp\;/g, '¤');
					var pf=value.toLowerCase().indexOf(search);
					if((pf >= 0) || (mselinfo.entries[s].className == 'msentrysel'))
					{
						disp=true;
						if(!entrysel)
							dec++;
						if(pf >= 0)
						{
							while(pf >= 0)
							{
								var ins='<font id="mselentrym_'+(s+1)+'" class="red">'+value.substr(pf,search.length)+'</font>';
								value=value.substr(0,pf)+ins+value.substr(pf+search.length);
								pf=value.toLowerCase().indexOf(search,pf+ins.length);
							}
						}
					}
					value=value.replace(/\¤/g, '&nbsp;');
				}
				else
				{
					disp=true;
					if(!entrysel)
						dec++;
				}
//dsplst += s+': id='+id+', value='+value+', disp='+disp+'\n';

				// Filter project type?
				if(id.length && (id != '0') && value.length && upst && (nomp || nosp || nots) && (mselinfo.entries[s].className != 'msentrysel') && disp)
				{
					les=mselinfo.entries[s].innerHTML.substr(mselinfo.entries[s].innerHTML.length-3,3);
					if(les == '**}')
					{
						if(nomp)
							disp=false;
					}
					else if(les == '{*}')
					{
						if(nosp)
							disp=false;
					}
					else if(nots)
						disp=false;
				}

				var msl=$('mselentryl_'+(s+1));
				var mss=$('mselentrys_'+(s+1));
				if(disp)
				{
					if(mss)
						mss.innerHTML=value;
					msl.style.display='';
				}
				else
					msl.style.display='none';
				if((!id.length || !value.length) && (upst && nosp && nots))
					msl.style.display='none';
				else if(search.length || (upst && (nomp || nosp || nots)))
				{
					if(!id.length || !value.length)
					{
						msl.style.display='none';
						if(!value.length)
							lel=s;
					}
					else if(disp && (lel >= 0))
					{
						if(fdl < 0)
							fdl=lel;
						for(var h=lel; h<s; h++)
						{
							var mhe=mselinfo.entries[h];
							if((h != fdl) && (!mhe.attributes || !mhe.attributes['svalue'] || !mhe.innerHTML.length))
								$('mselentryl_'+(h+1)).style.display='';
						}
						lel=-1;
					}
				}
			}
		}
	}
//alert(dsplst);

	// Scroll to selected entry
	if(entrysel)
		oID.selpop.scrollTop=dec*16;

	// Focus on search text
	if(kc == 13)
	{
		oID.selpopsearchtext.focus();
		oID.selpopsearchtext.select();
	}

	// Save project checkboxes
	if(pstv.length)
	{
		var pv='psps_'+oVar.lastSelUID+'=tx'+pstv;
		fxf_fn_saveFields(pv,'');
	}
}

function fxf_fn_multiselToggle(mode,element)
{
	// Delete
	if(mode < 0)
	{
		mselinfo.element=null;
		mselinfo.entries=null;

		oID.selpop.style.display='none';
		oID.selpopsearch.style.display='none';
		oID.selpoptoggle.style.display='none';
		oID.selpopptype.style.display='none';
	}
	// Create or Shrink
	if((mode == 0) || (mode == 1))
	{
		if(!mode && element)
		{
			fxf_fn_unfocus(true,true,null);
			selentries=[];
			mselinfo.element=element;
			mselinfo.entries=element.down().childElements();
//alert('element='+element+' (id='+element.id+')\nmselinfo.entries.length='+mselinfo.entries.length);
			mselinfo.dim=fxf_fn_getBCR(element);
			if(mselinfo.entries && mselinfo.entries.length)
			{
				for(var m=0; m<mselinfo.entries.length; m++)
					selentries[m]=mselinfo.entries[m].innerHTML;
//alert('mselinfo.entries.length='+mselinfo.entries.length+'\n\nselentries:\n'+selentries);
			}
//alert('mselinfo - mode='+mselinfo.mode+':\n\nid='+mselinfo.element.id+'\ndim.left='+mselinfo.dim.left+', dim.top='+mselinfo.dim.top+', dim.right='+mselinfo.dim.right+', dim.bottom='+mselinfo.dim.bottom+', dim.width='+mselinfo.dim.width+', dim.height='+mselinfo.dim.height);

			mode=2;

			var mand=element.className.substr(element.className.length-2,2);
			if(mand != 'mm')
				mand='';
			else
				mand='m';
			oID.selpopsearch.className='focuscls'+mand;
		}

		if((mode != 2) && mselinfo.entries && mselinfo.entries.length)
		{
			if(!mode)
				mode=1;
		}
		if((mode == 1) && element)
		{
			var edim=fxf_fn_getBCR(element);
			oID.selpop.style.display='none';
			oID.selpopsearch.style.display='none';

			$('selpoptoggleicon').src='GFX/enlarge.png';
			oID.selpoptoggle.style.left=(edim.right-1)+'px';
			oID.selpoptoggle.style.top=(edim.top-1)+'px';
			oID.selpoptoggle.style.borderColor='#bbbbbb';
			oID.selpoptoggle.style.display='';

			oVar.mSelElement=element;
		}
		else if(mode != 2)
		{
			mode=-1;

			mselinfo.element=null;
			mselinfo.entries=null;

			oID.selpop.style.display='none';
			oID.selpopsearch.style.display='none';
			oID.selpoptoggle.style.display='none';
			oID.selpopptype.style.display='none';
		}
	}
	// Enlarge
	if((mode == 2) && element)
	{
		var entrysel=-1;
		var h='<ul style="margin:0;padding:0;">\n';
		for(var m=0; m<selentries.length; m++)
		{
			var chk='';
			if(mselinfo.entries[m].className.substr(mselinfo.entries[m].className.length-3,3) == 'sel')
			{
				chk=' checked';
				if(entrysel < 0)
					entrysel=m;
			}
			var bg=mselinfo.entries[m].style.background;
			if(selentries[m].length && (mselinfo.entries[m].className != 'sdisabled'))
				h += '\t<li id="mselentryl_'+(m+1)+'" class="'+mselinfo.entries[m].className+'" style="margin:0;padding:0;background:'+bg+';"><input id="mselentryc_'+(m+1)+'" type="checkbox" value="1"'+chk+' style="position:relative;left:0;top:-2px;">&nbsp;<span id="mselentrys_'+(m+1)+'" style="position:relative;left:0;top:-4px;">'+selentries[m]+'</span></li>\n';
			else
				h += '\t<li id="mselentryl_'+(m+1)+'" class="sdisabled" style="margin:0;padding:0;"><span style="position:relative;left:26px;">'+selentries[m]+'</span></li>\n';
		}
		h += '</ul>\n';
		oID.selpop.innerHTML=h;

		if((element.className == 'focusfxfsm') || (element.className == 'focusfmfsm'))
			oID.selpop.className='selcolm';
		else
			oID.selpop.className='selcol';

		var ww=Math.min(mselinfo.dim.width+128, dim.wd.right-mselinfo.dim.left-22);
		oID.selpop.style.left=mselinfo.dim.left+'px';
		oID.selpop.style.top=(dim.wd.top+23)+'px';
		oID.selpop.style.right='';
		oID.selpop.style.bottom='';
		oID.selpop.style.width=ww+'px';
		oID.selpop.style.height=(dim.wd.bottom-dim.wd.top-25)+'px';
		oID.selpop.style.display='';

		oID.selpop.scrollTop=entrysel*16;

		oID.selpopsearchtext.value='';
		oID.selpopsearchtext.style.width=(ww-50)+'px';
		oID.selpopsearchtext.style.top='3px';
		oID.selpopsearch.style.left=mselinfo.dim.left+'px';
		oID.selpopsearch.style.top=(dim.wd.top)+'px';
		oID.selpopsearch.style.width=(ww-27)+'px';
		oID.selpopsearch.style.height='14px';
		oID.selpopsearch.style.display='';

		$('selpoptoggleicon').src='GFX/shrink.png';
		oID.selpoptoggle.style.left=(mselinfo.dim.left+ww-22)+'px';
		oID.selpoptoggle.style.top=(dim.wd.top)+'px';
		oID.selpoptoggle.style.borderColor='#000000';
		oID.selpoptoggle.style.display='';
	}
	mselinfo.mode=mode;
}

function fxf_fn_workScroll(event)
{
	// Select popup?
	fxf_fn_unfocus(true,true,null);
	// Text popup?
	if(oID.txtpoptoggle.style.display != 'none')
		oID.txtpoptoggle.style.display='none';
	if(oFXP.listha.length)
	{
		var wst=oID.fxwork.scrollTop;
		var wsl=oID.fxwork.scrollLeft;
		for(var lhc=0; lhc<oFXP.listha.length; lhc++)
		{
			var bcr=fxf_fn_getBCR(oFXP.listha[lhc].src);
			if(bcr.top < dim.wd.top)
			{
				if(!oFXP.listha[lhc].height)
				{
					var lhtab=oFXP.listha[lhc].src.up();
					var lhtabd=fxf_fn_getBCR(lhtab);
					oFXP.listha[lhc].height=lhtabd.height;
					var lhtds=oFXP.listha[lhc].src.childElements();
					var flh=$('flistheader'+lhc);
					if(flh)
					{
						var flhtds=$('flistheader'+lhc).childElements();
						if(lhtds.length && (lhtds.length == flhtds.length))
						{
							for(var c=0; c<lhtds.length; c++)
							{
								var tdd=fxf_fn_getBCR(lhtds[c]);
								flhtds[c].style.width=(tdd.width-9)+'px';
							}
						}
					}
				}
				oFXP.listha[lhc].div.style.left=(wsl+bcr.left-dim.wd.left)+'px';
				oFXP.listha[lhc].div.style.top=Math.min(wst,wst-dim.wd.top-bcr.height+4+bcr.top+oFXP.listha[lhc].height)+'px';
				oFXP.listha[lhc].div.style.width=bcr.width+'px';
				oFXP.listha[lhc].div.style.height=bcr.height+'px';
				oFXP.listha[lhc].div.style.display='';
			}
			else if(bcr.top >= dim.wd.top)
				oFXP.listha[lhc].div.style.display='none';
		}
	}
}

function fxf_fn_updateEstMethod(pid)
{
	var id_method=$('fg_method');
	// Change estimation method and customer depending on selected task
	if(id_method)
	{
		var url=fxf_fn_gProgram('stunden_tag_fg', '__itr=81&fg_pid='+pid+fxf_fn_gParam());
		new Ajax.Request(url,
		{
			method:'get',
			onSuccess: function(transport)
			{
				var rta=transport.responseText.split('|');
				var customer=0;
				if(rta.length > 1)
					customer=parseInt(rta[1]);
//alert('url='+url+'\n\ntransport.responseText='+transport.responseText+'\n\nrta='+rta+'\n\ncustomer='+customer);
				// Estimation method
				id_method.innerHTML=rta[0];
				// Customer
				if(customer > 0)
				{
					var id_customer=fxf_fn_getField($('Kunde'),true);
					if(id_customer)
						fxf_fn_changeSelectedValue(id_customer,customer,true);
				}
			}
		});
	}
}

function fxf_fn_updateCustomer(pid)
{
	var id_customer=fxf_fn_getField($('Kunde'),true);
	// Change customer depending on selected task
	if(id_customer)
	{
		var url=fxf_fn_gProgram('stunden_tag_fg', '__itr=81&fg_pid='+pid+fxf_fn_gParam());
		new Ajax.Request(url,
		{
			method:'get',
			onSuccess: function(transport)
			{
				var rta=transport.responseText.split('|');
				var customer=0;
				if(rta.length > 1)
				{
					customer=parseInt(rta[1]);
//alert('url='+url+'\n\ntransport.responseText='+transport.responseText+'\n\nrta='+rta+'\n\ncustomer='+customer);
					fxf_fn_changeSelectedValue(id_customer,customer,true);
				}
			}
		});
	}
	fxf_fn_waitScreen('');
}

function fxf_fn_updateSkills()
{
	if(oID.selpop.style.display == 'none')
	{
		var id_skills=fxf_fn_getElement('MaFaehigkeit_ID');
		var skills=fxf_fn_getSelectedArray('MaFaehigkeit_ID');
		if(id_skills && skills && skills.length)
		{
			// Check, if only one project is selected
			var id_projects=fxf_fn_getField($('Projekt_ID'),true);
			var sc=0;
			var st=0;
			if(id_projects.fxv && id_projects.fxv.length)
			{
				var ids=id_projects.fxv.split('|');
				sc=ids.length;
				st=parseInt(ids[0]);
			}
//alert('sc='+sc+', st='+st);

			// Check, if selected project is a task with defined skills
			if((sc == 1) && (st > 0))
			{
				var fspmoa=$$('[id^="fspmo_MaFaehigkeit_ID"]');
				for(var i=0; i<tSet.task_skills_array.length; i++)
				{
//alert(i+': st='+st+', project='+tSet.task_skills_array[i].project);
					if(st == tSet.task_skills_array[i].project)
					{
						if(tSet.task_skills_array[i].skill > 0)
							var so=tSet.task_skills_array[i].skill+'f';
						else
							var so=tSet.task_skills_array[i].skillgroup;
//alert('so='+so);

						var changed=false;
						id_skills.fxv='';
						for(var s=0; s<skills.length; s++)
						{
//alert('skill '+s+': no='+skills[s].no+', value='+skills[s].value+', selected='+skills[s].selected);
							if(skills[s].value == so)
							{
								if(!skills[s].selected)
								{
									changed=true;
									skills[s].selected=true;
									for(var f=0; f<fspmoa.length; f++)
									{
										if(fspmoa[f].attributes['svalue'].value == skills[s].value)
											fspmoa[f].className='msentrysel';
									}
//alert(s+' -> Select');
								}
							}
							else if(skills[s].selected)
							{
								changed=true;
								skills[s].selected=false;
								for(var f=0; f<fspmoa.length; f++)
								{
									if(fspmoa[f].attributes['svalue'].value == skills[s].value)
										fspmoa[f].className='msentry';
								}
//alert(s+' -> Deselect');
							}
							if(skills[s].selected)
							{
								if(id_skills.fxv.length)
									id_skills.fxv += '|';
								id_skills.fxv += skills[s].value;
							}
						}

						if((tSet.call_counter > 0) && (tSet.call_counter < 6) && changed)
						{
//alert('fxv='+id_skills.fxv);
							id_skills.attributes['svalue'].value=id_skills.fxv;
							fxf_fn_saveElement.delay(oFXP.ldelay, id_skills);

							if(oFXP.lang == 1)
								var ctext='Die Fähigkeit im Auswahlfeld wurde an die geänderte Aufgabe angepasst!';
							else
								var ctext='The skill in the selection field was adjusted to the changed task!';
							fxf_fn_question('', ctext, ['OK'], [''], 100);

							tSet.call_counter++;
						}

						break;
					}
				}
			}
		}

		if(!tSet.call_counter)
			tSet.call_counter=1;
	}
}

function fxf_fn_openResPPSP(persid, name, prj_pp, prj_sp, pers_pp, pers_sp, est_pp, est_sp, event)
{
	oID.iact.style.display='';
//alert('fxf_fn_openResPPSP(persid='+persid+', prj_pp='+prj_pp+', prj_sp='+prj_sp+', pers_pp='+pers_pp+', pers_sp='+pers_sp+', est_pp='+est_pp+', est_sp='+est_sp+')');
	tSet.sel_persid = 0;
	var ppe = fxf_fn_getElement('EK['+persid+']');
	var spe = fxf_fn_getElement('VK['+persid+']');
//alert('ppe='+ppe+', spe='+spe);
	var id_popup_res_ppsp = $('popup_res_ppsp');
	if(id_popup_res_ppsp)
	{
		$('id_name').innerHTML = name;

		if(ppe)
		{
			$('id_act_pp').innerHTML = ppe.value;
			$('id_std_pp').innerHTML = pers_pp;
			$('id_est_pp').innerHTML = est_pp;
			$('id_prj_pp').innerHTML = prj_pp;
		}
		else
		{
			$('id_act_pp').innerHTML = '-';
			$('id_std_pp').innerHTML = '-';
			$('id_est_pp').innerHTML = '-';
			$('id_prj_pp').innerHTML = '-';
		}
		if(spe)
		{
			$('id_act_sp').innerHTML = spe.value;
			$('id_std_sp').innerHTML = pers_sp;
			$('id_est_sp').innerHTML = est_sp;
			$('id_prj_sp').innerHTML = prj_sp;
		}
		else
		{
			$('id_act_sp').innerHTML = '-';
			$('id_std_sp').innerHTML = '-';
			$('id_est_sp').innerHTML = '-';
			$('id_prj_sp').innerHTML = '-';
		}

		oID.hrppsppop.style.left=Math.max(12,(mcx-360))+'px';
		oID.hrppsppop.style.top=Math.max(12,(mcy-90))+'px';
		oID.hrppsppop.style.display='';

		tSet.sel_persid=persid;
	}
}

function fxf_fn_setResPPSP(id_span)
{
	if(tSet.sel_persid)
	{
		var fs='';
		var form='';

		var ppe=fxf_fn_getElement('EK['+tSet.sel_persid+']');
		var spe=fxf_fn_getElement('VK['+tSet.sel_persid+']');
//alert('ppe='+ppe+', spe='+spe);

		if(ppe)
		{
			ppe.value=$('id_'+id_span+'_pp').innerHTML;
			if(!form.length)
				form=ppe.attributes['fxform'].value;
			fs += 'EK['+tSet.sel_persid+']=tx'+ppe.value;
		}
		if(spe)
		{
			spe.value=$('id_'+id_span+'_sp').innerHTML;
			if(!form.length)
				form=spe.attributes['fxform'].value;
			if(fs.length)
				fs += '&';
			fs += 'VK['+tSet.sel_persid+']=tx'+spe.value;
		}

		var ni='';
		if(id_span == 'std')
			ni='ok';
		else if(id_span == 'est')
			ni='ge';
		else if(id_span == 'prj')
			ni='dt';

		if(ni.length)
		{
			if(ppe)
				$('iconEK['+tSet.sel_persid+']').innerHTML=$('iek'+ni).innerHTML;
			if(spe)
				$('iconVK['+tSet.sel_persid+']').innerHTML=$('ivk'+ni).innerHTML;
		}

		tSet.sel_persid = 0;

		if(fs.length)
			fxf_fn_saveFields(fs, 'frm='+form);
	}
	fxf_fn_fxLinkClose();
}

function fxf_fn_getAssignedResources()
{
	var ars='';
	var cels=$$('[id^="OK"]');
	if(cels.length)
	{
		for(var c=0; c<cels.length; c++)
		{
			if(cels[c].checked)
				ars += '1';
			else
				ars += '0';
		}
	}
//alert('ars='+ars);

	return ars;
}

function fxf_fn_getSelectedArray(idname)
{
	var field=fxf_fn_getField($(idname),true);

	var sa=[];
	if(field && field.uid && field.uid.length)
	{
		var ssm=$('sel_'+field.uid);
		if(!ssm)
		{
			var ms=field.uid.indexOf('[');
			if(ms > 0)
				ssm=$('sel_'+field.uid.substr(0,ms));
		}
		if(ssm)
		{
			var html=fxf_fn_getSelPopup(field,ssm,field.attributes['svalue'].value);
			return fxf_fn_html2array(html);
		}
		else
		{
			var url=fxf_fn_worker('select','id='+field.uid+'&sel='+field.attributes['svalue'].value);
			new Ajax.Request(url,
			{
				method:'get', asynchronous:true,
				onSuccess: function(transport)
				{
//fxf_fn_writeDebug('log', transport.responseText);
					var html=transport.responseText;
					return fxf_fn_html2array(html);
				}
			});
		}
	}
	else
		return sa;
}

function fxf_fn_html2array(html)
{
	var id_seltmp=$('seltmp');
	id_seltmp.innerHTML=html.replace(/fspo\-/g, 'fspto-');

	var a=[];
	var sels=$$('[id^="fspto-"]');
	if(sels && sels.length)
	{
		for(var s=0; s<sels.length; s++)
		{
			var value='';
			if(sels[s].attributes && sels[s].attributes['svalue'])
				value=sels[s].attributes['svalue'].value;
			a[s]={'element':sels[s], 'no':parseInt(sels[s].id.substr(5)), 'value':value, 'text':sels[s].innerHTML, 'style':sels[s].style, 'selected':false};
			if(sels[s].className == 'sentrysel')
				a[s].selected=true;
		}
	}
	id_seltmp.innerHTML='';
	return a;
}

function fxf_fn_getSelType(element)
{
	var ecn=fxf_fn_getRealClassName(element, false);
	if(ecn.length)
		return ecn.substr(0,3);
	return 'fxf';
}

function fxf_fn_updateRTasks(pid, field, pdt, dmd)
{
	// Change task select element
	var tse=fxf_fn_getField($(field),true);
	if(tse)
	{
		var url=fxf_fn_gProgram('rtasks', 'pid='+pid+'&s_name='+field+'&pdatum='+pdt+'&ansicht='+dmd);
		var uadd=$('rtaskparam').innerHTML.split(',');
		var multi=parseInt(uadd[2]);
		url += '&multi='+multi+'&with_empty='+parseInt(uadd[3])+'&timereg='+parseInt(uadd[4])+'&with_unknown='+parseInt(uadd[5])+'&nocost='+parseInt(uadd[6])+'&notodos='+parseInt(uadd[7])+fxf_fn_gParam();
//alert('url='+url);
		new Ajax.Request(url,
		{
			method:'get', asynchronous:true,
			onSuccess: function(transport)
			{
				var ret=transport.responseText;
//alert('ret='+ret);
				if(multi)
				{
					tse.innerHTML=ret;
					fxf_fn_waitScreen('');
				}
				else
				{
					var opid=parseInt(tse.fxv);
					fxf_fn_setNewSelects(tse,ret,true);
					var npid=parseInt(tse.fxv);
					fxf_fn_ajaxSaveElement.delay(oFXP.mdelay, tse);
					if(opid != npid)
						fxf_fn_updateCustomer.delay(oFXP.ldelay, npid);
					else
						fxf_fn_waitScreen('');
				}
			}
		});
	}
	else
		fxf_fn_waitScreen('');
}

function fxf_fn_extractTime(f,v,nomaxval)
{
	if(nomaxval)
		nomaxval=true;

	var hr=fxf_fn_extractHour(f,v,nomaxval);
	var mn=fxf_fn_extractMinute(f,v,nomaxval);
	var t=hr*60+mn;

	if(!nomaxval)
		t=Math.min(t,1440);
//alert(f+': extractTime of ['+v+'] = '+t);

	return t;
}

function fxf_fn_extractHour(f,v,nomaxval)
{
	if(nomaxval)
		nomaxval=true;

	var t='';
	var hr=0;
	var mn=0;
	var cpos=false;
	if(v.length)
		cpos=v.indexOf(oSet.decimalpoint);
	if(cpos >= 0)
	{
		if(cpos > 0)
		{
			t=fxf_fn_cutLeadingZeros(v.substr(0,cpos));
			if(isNaN(t))
				t='0';
			hr=parseInt(t);
			if(!nomaxval)
			{
				if(f == 'st')
					hr=Math.min(hr,23);
				else
					hr=Math.min(hr,24);
			}
		}
	}
	else
	{
		if(v.length)
			cpos=v.indexOf(':');
		if(cpos >= 0)
		{
			if(cpos > 0)
			{
				t=fxf_fn_cutLeadingZeros(v.substr(0,cpos));
				if(isNaN(t))
					t='0';
				hr=parseInt(t);
				if(!nomaxval)
				{
					if(f == 'st')
						hr=Math.min(hr,23);
					else
						hr=Math.min(hr,24);
				}
			}
		}
		else if(f == 'br')
		{
			t=fxf_fn_cutLeadingZeros(v);
			if(isNaN(t))
				t='0';
			mn=parseInt(t);
			if(mn > 59)
			{
				hr=Math.floor(mn/60);
				if(!nomaxval)
					hr=Math.min(hr, 24);
			}
		}
		else
		{
			t=fxf_fn_cutLeadingZeros(v);
			if(isNaN(t))
				t='0';
			hr=parseInt(t);
			if(!nomaxval)
			{
				if(f == 'st')
					hr=Math.min(hr,23);
				else
					hr=Math.min(hr,24);
			}
		}
	}
//alert(f+': extractHour of ['+v+'] = '+hr);

	return hr;
}

function fxf_fn_extractMinute(f,v,nomaxval)
{
	if(nomaxval)
		nomaxval=true;

	var t='';
	var hr=0;
	var mn=0;
	var cpos=false;
	if(v.length)
		cpos=v.indexOf(oSet.decimalpoint);
	if(cpos >= 0)
	{
		if(cpos < v.length-1)
		{
			t=fxf_fn_cutLeadingZeros(v.substr(cpos+1));
			if(isNaN(t))
				t='0';
			mn=parseInt(parseFloat("0."+t) * 60);
		}
	}
	else
	{
		if(v.length)
			cpos=v.indexOf(':');
		if(cpos >= 0)
		{
			if(cpos < v.length-1)
			{
				t=fxf_fn_cutLeadingZeros(v.substr(cpos+1));
				if(isNaN(t))
					t='0';
				mn=Math.min(parseInt(t), 59);
			}
		}
		else if(f == 'br')
		{
			t=fxf_fn_cutLeadingZeros(v);
			if(isNaN(t))
				t='0';
			mn=parseInt(t);
			if(mn > 59)
			{
				var hr=Math.floor(mn/60);
				if(!nomaxval)
					hr=Math.min(hr,24);
				mn=Math.min(mn-hr*60, 59);
			}
		}
	}
//alert(f+': extractMinute of ['+v+'] = '+mn);

	return mn;
}

function fxf_fn_makeTime(f,t)
{
	var it=parseInt(t);
	var h=Math.floor(it/60);
	var m=it-h*60;

	return fxf_fn_makeHour(f,h)+':'+fxf_fn_makeMinute(f,m);
}

function fxf_fn_makeHour(f,h)
{
	var ih=parseInt(h);
	if((ih < 10) && ((f == 'st') || (f == 'en')))
		h='0'+ih;

	return h;
}

function fxf_fn_makeMinute(f,m)
{
	var im=parseInt(m);
	if(im < 10)
		m='0'+im;

	return m;
}

function fxf_fn_changePD(element)
{
	if(oFXP.action != 3)
		return;
	var sci=$('subchange');
	if(!sci)
		return;
//alert('fxf_fn_changePD('+element.id+')');

	var ei=element.id;
	if(ei == 'tage')
		var lit=$('lit_zeitpunkt').innerHTML;
	else
		var lit=$('lit_'+ei).innerHTML;
	var cp=lit.indexOf(':');
	if(cp)
		lit=lit.substr(0,cp);

	if(oFXP.lang == 1)
		fxf_fn_question('Untergeordnete Elemente abändern?', '<b>'+lit+'</b> wurde abgeändert.<br /><br /><br />Sollen die untergeordneten Elemente ebenfalls abgeändert werden?', ['Nein', 'Ja, nur gleiche Elemente', 'Ja, alle'], ['fxf_fn_changePDS(0,\''+ei+'\');', 'fxf_fn_changePDS(1,\''+ei+'\');', 'fxf_fn_changePDS(2,\''+ei+'\');'], 0);
	else
		fxf_fn_question('Change Subordinate Elements', '<b>'+lit+'</b> has been changed.<br /><br /><br />Should the subordinate elements also be changed?', ['No', 'Yes, only equal elements', 'Yes, all'], ['fxf_fn_changePDS(0,\''+ei+'\');', 'fxf_fn_changePDS(1,\''+ei+'\');', 'fxf_fn_changePDS(2,\''+ei+'\');'], 0);
}

function fxf_fn_changePDS(a,ei)
{
	a=parseInt(a);
	var sci=$('subchange');
	var osc=sci.value;
	var nsc='';

	var tad='';
	if(ei == 'tage')
	{
		ei='zeitpunkt';
		var tri=document.getElementsByName('zeitpunkt');
		if(!tri[2].checked)
		{
			tad='&zeitpunkt=tx1792';
			tri[2].checked=true;
		}
	}

	var nsd=false;
	if(osc.length)
	{
		var oss=osc.split('|');
		for(var o=0; o<oss.length; o++)
		{
			var ea=oss[o].split(':');
			if(nsc.length)
				nsc += '|';
			if(ea[0] == ei)
			{
				nsd=true;
				if(a > 0)
					nsc += ei+':'+a;
			}
			else
				nsc += oss[o];
		}
	}

	if(!nsd && (a > 0))
	{
		if(nsc.length)
			nsc += '|';
		nsc += ei+':'+a;
	}
//alert('fxf_fn_changePDS(a='+a+', ei='+ei+')\nosc='+osc+'\nnsc='+nsc);

	if((nsc != osc) || tad.length)
	{
		sci.value=nsc;
		var form=sci.attributes['fxform'].value;
		var fs='subchange=tx'+nsc+tad;
//alert('form: '+form+'\nfs: '+fs);
		fxf_fn_saveFields(fs, 'frm='+form);
	}
}

function fxf_fn_loadManual(tr,pdf)
{
	var url=fxf_fn_gProgram('popup_hlp', 'mtr='+tr+fxf_fn_gParam());
	var hbtd=$('hb_txt');
//alert('url='+url+'\nhbtd='+hbtd);
	if(hbtd)
	{
		fxf_fn_waitScreen('loading');
		if(!pdf || !pdf.length)
		{
			new Ajax.Request(url,
			{
				method:'get', asynchronous:true,
				onSuccess: function(transport)
				{
					hbtd.innerHTML=transport.responseText;
					fxf_fn_waitScreen('');
				}
			});
		}
		else
		{
			if(oFXP.pwindow)
				oFXP.pwindow.close();

			var iframe='<iframe id="fm_display_iframe" src="'+pdf+'#'+fxf_fn_today(true)+'" width="100%" height="100%" marginwidth="0" marginheight="0" hspace="0" vspace="0" frameborder="0" style="border:0;margin:0;padding:0;"></iframe>';
			var psw=oFXP.pwindow=window.open('','pdfwindow','left='+(200+screen.availLeft)+',top=100,width='+(dim.sd.pwidth-400)+',height='+(dim.sd.pheight-200)+',toolbar=no,location=no,directories=no,status=no,menubar=no,scrollbars=yes,copyhistory=no,resizable=yes');
			psw.document.write('<html><head><title>PDF Manual</title></head><body>'+iframe+'</body></html>');
			fxf_fn_waitScreen('');
			oFXP.pwindow.focus.delay(oFXP.sdelay);
		}
	}
}

function fxf_fn_changeReg(reg)
{
	var rels=$$('[id^="hb_r"]');
	if(rels.length)
	{
		for(var r=0; r<rels.length; r++)
		{
			var rt=rels[r].id.substr(4);
			var mn=$('hb_m'+rt);
//alert('rt='+rt+', mn='+mn);
			if(rt == reg)
			{
				rels[r].className='rega';
				if(mn)
					mn.style.display='';
			}
			else
			{
				rels[r].className='regi';
				if(mn)
					mn.style.display='none';
			}
		}
	}
}

function fxf_fn_toggleSection(sectionname)
{
	var idsection=$('section_'+sectionname);
	var idicon=$('icon_'+sectionname);
	if(idsection.style.display == 'none')
	{
		idsection.style.display='';
		idicon.src = 'GFX/fl_cl.png';
	}
	else
	{
		idsection.style.display='none';
		idicon.src = 'GFX/fl_op.png';
	}
}

function fxf_fn_toggleTimespan()
{
	var id_from=fxf_fn_getElement('Zeitspanne_von');
	var id_to=fxf_fn_getElement('Zeitspanne_bis');
//alert('id_from='+id_from+', id_to='+id_to);

	if(id_from && id_to)
	{
		var id_auto=fxf_fn_getElement('AutoZeitspanne');
		if(id_auto.checked)
		{
			id_from.disabled=true;
			id_to.disabled=true;
		}
		else
		{
			id_from.disabled=false;
			id_to.disabled=false;
		}
	}
}

function fxf_fn_updateTimespan(element)
{
	fxf_fn_toggleTimespan();

	var id_auto=fxf_fn_getElement('AutoZeitspanne');
	var auto=0;
	if(id_auto.checked)
		auto=1;

	if(auto)
	{
		var id_from=fxf_fn_getElement('Zeitspanne_von');
		var id_to=fxf_fn_getElement('Zeitspanne_bis');

		if(id_from && id_to)
		{
			fxf_fn_waitScreen('#');
			var id_pid=fxf_fn_getElement('ProjektID');
			var id_lvl=fxf_fn_getElement('Level');
			var id_jplan=fxf_fn_getElement('nur_plandaten');

			var pid=parseInt(fxf_fn_getSelectedValue(id_pid).value);
			var lvl=id_lvl.value;
			var jplan=0;
			if(id_jplan.checked)
				jplan=1;

			var url=fxf_fn_gProgram('projektstatus', 'pid='+pid+'&auto='+auto+'&lvl='+lvl+'&jplan='+jplan+fxf_fn_gParam());
//alert('url='+url);

			new Ajax.Request(url,
			{
				method: 'get',
				onSuccess: function(transport)
				{
					var from_to=transport.responseText.split('|');
					id_from.value=from_to[0];
					id_from.fxv=id_from.value;
					id_to.value=from_to[1];
					id_to.fxv=id_to.value;

					// Save changes
					fxf_fn_saveFields('Zeitspanne_von=tx'+id_from.fxv+'&Zeitspanne_bis=tx'+id_to.fxv, 'frm='+id_from.attributes['fxform'].value);
					if(achg)
						fxf_fn_waitScreen('');
				}
			});
		}
	}
}

function fxf_fn_toggleSubs(l, svn)
{
	var frm='';
	var dchanged='';
	var elems=$$('[id="its'+l+'"]');
	var slvl=1+fxf_fn_countSubstrings(svn, '.');
	var ipt=elems[0].src.substr(0, elems[0].src.length-6);
	var img=elems[0].src.substr(elems[0].src.length-6, 2);
//alert('fxf_fn_toggleSubs('+l+', \''+svn+'\') - slvl='+slvl+'\n\ne='+elems+', elems[0].src='+elems[0].src+'\nipt='+ipt+', img='+img);
	if(img == 'op')
	{
		img = 'cl';
		var enl = true;
	}
	else
	{
		img = 'op';
		var enl = false;
	}
	for(var i=0; i<elems.length; i++)
		elems[i].src=ipt+img+'.png';

	var id_pr=$('pr['+l+']');
	if(id_pr)
	{
		if(id_pr.attributes && (typeof id_pr.attributes['fxform'] != 'undefined'))
			frm=id_pr.attributes['fxform'].value;
		$('disp'+id_pr.value).value='_'+img;
		if(dchanged.length)
			dchanged += '&';
		dchanged += 'disp'+id_pr.value+'=tx_'+img;
	}

	var id_vn=null;
	var id_tr=null;
	var id_im=null;
	var vn='';
	var dlvl=0;
	while(true)
	{
		l++;
		id_vn=$('vn['+l+']');
		if(!id_vn)
			break;

		vn=id_vn.value;
		dlvl=fxf_fn_countSubstrings(vn, '.');
		if(dlvl < slvl)
			break;

		if((dlvl == slvl) || (!enl && (dlvl > slvl)))
		{
//alert('Found subline '+l+': value='+vn+', level='+dlvl);
			id_tr=$$('[id="tr'+l+'"]');
			if(id_tr && id_tr.length)
			{
				for(i=0; i<id_tr.length; i++)
				{
					if(enl)
						id_tr[i].style.display='';
					else
						id_tr[i].style.display='none';
				}

				img='';
				id_im=$$('[id="its'+l+'"]');
				if(id_im && id_im.length)
				{
					if(enl)
						img='op';
					else
						img='cl';
					for(i=0; i<id_im.length; i++)
						id_im[i].src=ipt+img+'.png';
				}

				id_pr=$('pr['+l+']');
				if(id_pr)
				{
					if(dchanged.length)
						dchanged += '&';
					if(enl)
					{
						$('disp'+id_pr.value).value='_'+img;
						dchanged += 'disp'+id_pr.value+'=tx_'+img;
					}
					else
					{
						$('disp'+id_pr.value).value='none_'+img;
						dchanged += 'disp'+id_pr.value+'=txnone_'+img;
					}
				}
			}
		}
	}

	if(dchanged.length && frm.length)
	{
//alert('frm:\n'+frm+'\n\ndchanged:\n'+dchanged);
		fxf_fn_saveFields(dchanged, 'frm='+frm);
	}

}

function fxf_fn_countSubstrings(src, search)
{
	var cnt=0;
	var pos=0;
	do{
		pos=src.indexOf(search, pos);
		if(pos > -1)
		{
			cnt++;
			pos++;
		}
		else
			break;
	} while(true);

	return cnt;
}

function fxf_fn_sortColumn(column)
{
	var spid=fxf_fn_getElement('sort_post');
	if(spid)
	{
		var frm='';
		if(spid.attributes && (typeof spid.attributes['fxform'] != 'undefined'))
			frm=spid.attributes['fxform'].value;

		fxf_fn_waitScreen('loading');
		fxf_fn_loadTR(oFXP.tr, 'Button_Submit&srtpst='+column+'&frm='+frm);
	}
}

function fxf_fn_toggleFields(el, tmode)
{
	if(!el)
		el=$('shfields');

	if(el)
	{
		var isr=el.src.substr(0,el.src.length-6);
		var ias=el.src.substr(el.src.length-6,2);

		if(tmode < 0)		// Toggle
		{
			if(ias == 'cl')
				tmode=0;
			else
				tmode=1;
		}
		if(tmode > 0)		// Show
			ias='cl';
		else				// Hide
			ias='op';
		el.src=isr+ias+'.png';

		if(oFXP.tr == 102)	// Material Usage
		{
			var trb=$('td_lit_Beschreibung');
			if(trb)
			{
				trb=trb.up('table');
				if(trb)
				{
					if(tmode)
						trb.style.display='';
					else
						trb.style.display='none';
				}
			}
			var tra=$('td_lit_Anmerkung');
			if(tra)
			{
				tra=tra.up('table');
				if(tra)
				{
					if(tmode)
						tra.style.display='';
					else
						tra.style.display='none';
				}
			}
		}
	}
}

function fxf_fn_changeBudgetApprovalCB(element)
{
	var ei=element.id.substr(8);
	var en=$('ppath'+ei).innerHTML;
	var ec=element.checked;
	var cels=$$('[id^="ppath"]');
//alert('ei='+ei+', en='+en+', ec='+ec+'\n\ncels: length='+cels.length+'\n'+cels);

	var fs='';
	if(cels.length)
	{
		for(var c=0; c<cels.length; c++)
		{
			var ci=cels[c].id.substr(5);
			var cn=cels[c].innerHTML;
			var cc=fxf_fn_getElement('Checkbox'+ci);
			if(cn != en)
			{
				if((cn.length < en.length) && (en.substr(0,cn.length) == cn))
				{
					if(!ec && cc.checked)
					{
						cc.checked=ec;
//alert(c+': PARENT - ci='+ci+', cn='+cn+' -- checked='+cc.checked);
						if(fs.length)
							fs += '&';
						fs += 'Checkbox'+ci+'=cb'+ec;
					}
				}
				else if((en.length < cn.length) && (cn.substr(0,en.length) == en))
				{
					if(cc.checked != ec)
					{
						cc.checked=ec;
//alert(c+': CHILD - ci='+ci+', cn='+cn+' -- checked='+cc.checked);
						if(fs.length)
							fs += '&';
						fs += 'Checkbox'+ci+'=cb'+ec;
					}
				}
			}
		}
	}
	if(fs.length)
		fxf_fn_saveFields.delay(oFXP.ldelay, fs, 'frm='+element.attributes['fxform'].value);
}

function fxf_fn_checkDMMDates(element)
{
	var v=element.value;
	var d=fxf_fn_convertDate2Timestamp(v).timestamp;
	var c=fxf_fn_convertTimestamp2Date(d).date;
//alert('fxf_fn_checkDMMDates('+element.id+'): v='+v+', d='+d+'\nminstart='+tSet.minstart+', maxstart='+tSet.maxstart+' - minend='+tSet.minend+', maxend='+tSet.maxend);

	if(element.id == 'Soll_Beg_Dtm')
	{
		var selement=fxf_fn_getElement('Soll_End_Dtm');
		var sv=selement.value;
		var sd=fxf_fn_convertDate2Timestamp(sv).timestamp;

		if((tSet.minstart != '00000000') && (d < tSet.minstart))
		{
			d=tSet.minstart;
			element.value=fxf_fn_convertTimestamp2Date(d).date;
			if(oFXP.lang == 1)
				fxf_fn_question('Datumsanpassung', '<b class="red">Dieses Datum '+c+' ist kleiner als das Minimum-Startdatum!</b><br /><br />Aufgrund der Abhängigkeiten dieses Projekts darf das gepl. Startdatum<br />nicht kleiner sein als <b>'+element.value+'</b>.<br /><br />Das gepl. Startdatum wurde dementsprechend abgeändert.', ['OK'], [''], 100);
			else
				fxf_fn_question('Date Adjustment', '<b class="red">This date '+c+' is less than the minimum start date!</b><br /><br />Because of dependencies of this project the planned start date<br />cannot be less than <b>'+element.value+'</b>.<br /><br />The planned start date has been adjusted.', ['OK'], [''], 100);
		}
		else if((tSet.maxstart != '99999999') && (d > tSet.maxstart))
		{
			d=tSet.maxstart;
			element.value=fxf_fn_convertTimestamp2Date(d).date;
			if(oFXP.lang == 1)
				fxf_fn_question('Datumsanpassung', '<b class="red">Dieses Datum '+c+' ist grösser als das Maximum-Startdatum!</b><br /><br />Aufgrund der Abhängigkeiten dieses Projekts darf das gepl. Startdatum<br />nicht grösser sein als <b>'+element.value+'</b>.<br /><br />Das gepl. Startdatum wurde dementsprechend abgeändert.', ['OK'], [''], 100);
			else
				fxf_fn_question('Date Adjustment', '<b class="red">This date '+c+' is greater than the maximum start date!</b><br /><br />Because of dependencies of this project the planned start date<br />cannot be greater than <b>'+element.value+'</b>.<br /><br />The planned start date has been adjusted.', ['OK'], [''], 100);
		}

		if(sv.length && (sd < d))
			selement.value=fxf_fn_convertTimestamp2Date(d).date;
	}
	else
	{
		var selement=fxf_fn_getElement('Soll_Beg_Dtm');
		var sv=selement.value;
		var sd=fxf_fn_convertDate2Timestamp(sv).timestamp;

		if((tSet.minend != '00000000') && (d < tSet.minend))
		{
			d=tSet.minend;
			element.value=fxf_fn_convertTimestamp2Date(d).date;
			if(oFXP.lang == 1)
				fxf_fn_question('Datumsanpassung', '<b class="red">Dieses Datum '+c+' ist kleiner als das Minimum-Enddatum!</b><br /><br />Aufgrund der Abhängigkeiten dieses Projekts darf das gepl. Enddatum<br />nicht kleiner sein als <b>'+element.value+'</b>.<br /><br />Das gepl. Enddatum wurde dementsprechend abgeändert.', ['OK'], [''], 100);
			else
				fxf_fn_question('Date Adjustment', '<b class="red">This date '+c+' is less than the minimum end date!</b><br /><br />Because of dependencies of this project the planned end date<br />cannot be less than <b>'+element.value+'</b>.<br /><br />The planned end date has been adjusted.', ['OK'], [''], 100);
		}
		else if((tSet.maxend != '99999999') && (d > tSet.maxend))
		{
			d=tSet.maxend;
			element.value=fxf_fn_convertTimestamp2Date(d).date;
			if(oFXP.lang == 1)
				fxf_fn_question('Datumsanpassung', '<b class="red">Dieses Datum '+c+' ist grösser als das Maximum-Enddatum!</b><br /><br />Aufgrund der Abhängigkeiten dieses Projekts darf das gepl. Enddatum<br />nicht grösser sein als <b>'+element.value+'</b>.<br /><br />Das gepl. Enddatum wurde dementsprechend abgeändert.', ['OK'], [''], 100);
			else
				fxf_fn_question('Date Adjustment', '<b class="red">This date '+c+' is greater than the maximum end date!</b><br /><br />Because of dependencies of this project the planned end date<br />cannot be greater than <b>'+element.value+'</b>.<br /><br />The planned end date has been adjusted.', ['OK'], [''], 100);
		}

		if(sv.length && (sd > d))
			selement.value=fxf_fn_convertTimestamp2Date(d).date;
	}

	if(element.value != v)
	{
		element.fxv=element.value;
		fxf_fn_ajaxSaveElement.delay(oFXP.ldelay, element);
	}

	if(selement.value != sv)
	{
		selement.fxv=selement.value;
		fxf_fn_ajaxSaveElement.delay(oFXP.sdelay*2.0, selement);
	}
}

function fxf_fn_convertHTMLEntities(s, keepImages)
{
	var ir='';
	if(keepImages)
	{
		var ii=s.indexOf('<img');
		if(ii >= 0)
		{
			var ie=s.indexOf('>', ii+4);
			var ir=s.substring(ii,ie+1);
			s=s.replace(/\<img.*\>/g, '¨');
		}
	}
	oID.taconv.innerHTML=s.stripTags().replace(/</g,'&lt;').replace(/>/g,'&gt;');
	if(keepImages && ir.length)
		oID.taconv.innerHTML = oID.taconv.innerHTML.replace(/\¨/g, ir);

	return oID.taconv.value;
}

function fxf_fn_strcmp(a,b)
{
	a=a.toString().toLowerCase(), b=b.toString().toLowerCase();
    for(var i=0,n=Math.max(a.length, b.length); (i<n) && (a.charAt(i) === b.charAt(i)); ++i);
    if(i === n)
    	return 0;
    return (a.charAt(i) > b.charAt(i)) ? -1 : 1;
}

function fxf_fn_setFilter()
{
	var flt=$('fxflt01');
	flt.src='GFX/it_filter_'+gSet.filter+'.png';
}

function fxf_fn_toggleEmptyInfo(mn)
{
//alert('fxf_fn_toggleEmptyInfo: mn='+mn);
	var dm=$('mtem_'+mn);
	if(dm)
	{
		var di=$('mtei_'+mn);
		if(dm.style.display == 'none')
		{
			di.src='GFX/fl_cl.png';
			dm.style.display='';
		}
		else
		{
			di.src='GFX/fl_op.png';
			dm.style.display='none';
		}
	}
}

function fxf_fn_growWorkflow(dur)
{
//alert('fxf_fn_growWorkflow(dur='+dur);

	gSet.wfsl=1;

	$('pwaitscreen').style.display='';
	oID.txtpoptoggle.style.display='none';

	oID.iworkflow.style.zIndex='9998';
	oID.iprojects.style.zIndex='9997';

	var wsh=Math.floor((dim.wd.height-64)/2.4);
	var wst=Math.max(32,Math.floor((wsh-oVar.dimiprojectsat.height)/2 + 8));

	var wf=fxf_fn_getBCR(oID.iworkflow);
	var wfi=fxf_fn_getBCR(oID.iworkflowi);
	var md=Math.min(Math.floor((dim.sd.pwidth-64)/2.75), Math.floor((wfi.height-10)/2.75));
	var mw=parseInt(md*2.75 + 10);
//alert('wf: left='+wf.left+', right='+wf.right+', top='+wf.top+', bottom='+wf.bottom+' -- width='+wf.width+', height='+wf.height+'\n\nwfi: left='+wfi.left+', right='+wfi.right+', top='+wfi.top+', bottom='+wfi.bottom+' -- width='+wfi.width+', height='+wfi.height+'\n-> md='+md);

	var id_wc=$('wfmwc');
	if(!id_wc || (gSet.wfradius != md) || (wf.width < mw))
		oID.iworkflow.style.width=mw+'px';

	oID.iworkflow.style.display='';
	oID.iworkflowai.style.backgroundPosition='-12px 0';
	new Effect.Morph(oID.iworkflow, {style:'left:0; opacity:1.0;', duration: dur});
	new Effect.Morph(oID.iworkflowa, {style:'height:'+wsh+'px;', duration: dur});
	new Effect.Morph(oID.iworkflowat, {style:'bottom:'+wst+'px;', duration: dur});

	fxf_fn_shrinkProjects(dur, 20);

	if(!id_wc || (gSet.wfradius != md))
		fxf_fn_drawWFMenu.delay(dur, md,false);
}

function fxf_fn_shrinkWorkflow(dur, la,pwd)
{
//alert('fxf_fn_shrinkWorkflow(dur='+dur+', la='+la+')');
	var a=-1;
	if(la > 0)
		a=la;

	if(oID.fxflt.style.display != 'none')
	{
		gSet.wfsl=0;

		if(la < 0)
			fxf_fn_shrinkProjects(dur,0);

		oID.iworkflow.style.display='';
		oID.iworkflowai.style.backgroundPosition='-36px 0';
		var nl=-(parseInt(oID.iworkflow.style.width)+a);
		new Effect.Morph(oID.iworkflow, {style:'left:'+nl+'px; opacity:0.5;', duration: dur});
		new Effect.Morph(oID.iworkflowa, {style:'height:32px;', duration: dur});
		new Effect.Morph(oID.iworkflowat, {style:'bottom:32px;', duration: dur});

		if(pwd != undefined)
			$('pwaitscreen').style.display=pwd;
		else if(!la)
			$('pwaitscreen').style.display='none';
	}
	else
		fxf_fn_shrinkWorkflow.delay(5.0, dur,la);
}


function fxf_fn_growProjects(dur,mpn,srt,opn)
{
//alert('fxf_fn_growProjects(dur='+dur+', mpn='+mpn+', srt='+srt+', opn='+opn+')');

	var pjsmm=$('pjsmm');
	var ampn='';
	if(pjsmm)
		ampn=pjsmm.value;

	if(oSet.sort09 == 'j')
	{
		$('pjsmc_srt09').style.display='';
		$('pjsmc_srtaz').style.display='none';
	}
	else
	{
		$('pjsmc_srt09').style.display='none';
		$('pjsmc_srtaz').style.display='';
	}

	var pfo='';
	var pcs='';

	gSet.prjsl=1;

	if(!mpn.length)
	{
		$('pwaitscreen').style.display='';
		oID.txtpoptoggle.style.display='none';

		oID.iworkflow.style.zIndex='9997';
		oID.iprojects.style.zIndex='9998';

		var wsh=Math.floor((dim.wd.height-64)/2.4);
		var wst=Math.max(32,Math.floor((wsh-oVar.dimiprojectsat.height)/2 + 8));

		var ipl=$('iprojectsl');
		ipl.style.display='';
		var opms='';
		var pms=$('iprojectss');
		if(pms)
		{
			opms=pms.innerHTML;
			pms.innerHTML='<font class="s2 grey">loading&hellip;</font>';
		}
		oID.iprojects.style.display='';
		oID.iprojectsai.style.backgroundPosition='-12px 0';
		new Effect.Morph(oID.iprojects, {style:'left:0; opacity:1.0;', duration: dur});
		new Effect.Morph(oID.iprojectsa, {style:'height:'+wsh+'px;', duration: dur});
		new Effect.Morph(oID.iprojectsat, {style:'top:'+wst+'px;', duration: dur});

		fxf_fn_shrinkWorkflow(dur, 20);

		var pels=$$('[id^="pjsmd_"]');
		if(pels.length)
		{
			for(var pc=0; pc<pels.length; pc++)
			{
				var i=pels[pc].id.substr(6);
				var si=$('pjsmfi_'+i);
				if(si)
				{
					var sm=si.src.substr(si.src.length-6,2);
					if(sm == 'fo')
					{
						if(pfo.length)
							pfo += '|';
						pfo += i;
					}
				}
			}
		}

		var pjscs=$('pjscs');
		if(pjscs)
			pcs=pjscs.value;
	}
	else if(ampn.length && (ampn != mpn))
	{
		var mpp=$('pjsmp_'+ampn);
		if(mpp)
		{
			mpp.style.borderRadius='';
			mpp.style.boxShadow='';
			mpp.style.padding='0';
			pjsmm.value='';
//alert('Unmark old project: '+ampn+', mpp='+mpp);
		}
	}

	var url=fxf_fn_gProgram('sld_projects', 'mpn='+mpn+'&srt='+srt+'&opn='+opn+fxf_fn_gParam());
	var pst='src='+fxf_fn_escapeText($('pjsmt_src').value,false)+'&pfo='+pfo+'&pcs='+pcs;
//alert('url='+url+'\n\npst='+pst);

	new Ajax.Request(url,
	{
		method:'post', postBody:pst, asynchronous:true,
		onSuccess: function(transport)
		{
			var pm=transport.responseText;
//alert('pm='+pm);
			if(!mpn.length)
			{
				if(pm.length)
				{
					if(opn.length < 4)
						oID.iprojectsm.innerHTML=pm;
					else
					{
						if(opn.substr(0,1) == '!')
							var rpn=opn.substr(1);
						else
							var rpn=opn;
						var pd=$('pjsmd');
						var api=$('pjsmd_'+rpn);
						if(pd && api)
						{
							var pels=$$('[id^="pjsmd_'+rpn+'."]');
							if(pels.length)
							{
								for(var pc=0; pc<pels.length; pc++)
									pd.removeChild(pels[pc]);
							}
							api.outerHTML=pm;
						}
					}
				}
				if(pms)
				{
					if(pm.length)
					{
						var pmi=$('pjsmi');
						if(pmi)
							pms.innerHTML=pmi.innerHTML;
					}
					else
						pms.innerHTML=opms;
				}
				ipl.style.display='none';
			}
			else if(mpn != '-')
			{
				var mpp=$('pjsmp_'+mpn);
				if(mpp)
				{
					mpp.style.borderRadius='4px';
					mpp.style.boxShadow='0 0 4px #006b9f';
					mpp.style.padding='0 4px';
					pjsmm.value=mpn;
//alert('Mark new project: '+mpn+', mpp='+mpp);
				}
			}
		}
	});
}

function fxf_fn_shrinkProjects(dur, la,pwd)
{
	var a=-1;
	if(la > 0)
		a=la;

	if(oID.fxflt.style.display != 'none')
	{
		gSet.prjsl=0;

		if(la < 0)
			fxf_fn_shrinkWorkflow(dur,0);

		oID.iprojects.style.display='';
		oID.iprojectsai.style.backgroundPosition='-36px 0';
		var nl=-(parseInt(oID.iprojects.style.width)+a);
		new Effect.Morph(oID.iprojects, {style:'left:'+nl+'px; opacity:0.5;', duration: dur});
		new Effect.Morph(oID.iprojectsa, {style:'height:32px;', duration: dur});
		new Effect.Morph(oID.iprojectsat, {style:'top:32px;', duration: dur});

		if(pwd != undefined)
			$('pwaitscreen').style.display=pwd;
		else if(!la)
			$('pwaitscreen').style.display='none';
	}
	else
		fxf_fn_shrinkProjects.delay(5.0, dur,la);
}

function fxf_fn_togglePJSMFolder(id,all)
{
//alert('fxf_fn_togglePJSMFolder(id='+id+', all='+all+')');
	var fa=$('pjsmfd_'+id);
	if(!fa)
		return;

	var il=id.length;
	var fi=$('pjsmfi_'+id);
	var fm=fi.src.substr(fi.src.length-6,2);
//alert('fm='+fm);

	// Open subfolder
	if(fm == 'fc')
	{
		var spid=id;
		if(all)
			spid='!'+id;
		fxf_fn_growProjects(0,'','',spid);
	}
	// Close subfolder
	else
	{
		var pels=$$('[id^="pjsmd_'+id+'."]');
		if(pels.length)
		{
			for(var pc=0; pc<pels.length; pc++)
			{
				var i=pels[pc].id.substr(6);
				var si=$('pjsmfi_'+i);
				if(si)
				{
					var sm=si.src.substr(si.src.length-6,2);
					if(sm == 'fo')
						si.src=si.src.substr(0,si.src.length-6)+'fc.png';
				}
				pels[pc].style.display='none';
			}
		}
		fi.src=fi.src.substr(0,fi.src.length-6)+'fc.png';
		if(id.length == 4)
		{
			var sp=$('pjsms_'+id);
			if(sp)
				sp.style.display='none';
		}
	}
}

function fxf_fn_openPJSMMenu(event,pn)
{
	var ii=$('pjsmfi_'+pn);
	if(!ii)
		return;
	var ic=ii.src.substr(ii.src.length-5,1);
	var ts=true;
	var st=0;
	if((ic == 'o') || (ic == 'c'))
		ts=false;
	else
		st=parseInt(ii.src.substr(ii.src.length-7,3));
//alert('Project action menu for: '+pn+', ic='+ic+', ts='+ts);

	var pa=pn.split('.');
	if(pa.length > 1)
	{
		var npn=fxf_fn_cutLeadingZeros(pa[0]);
		for(var p=1; p<pa.length; p++)
			npn += '.'+fxf_fn_cutLeadingZeros(pa[p]);
	}
	else
		var npn=fxf_fn_cutLeadingZeros(pn);

	var id='';
	var tx='';
	var tr='<tr style="height:20px;cursor:pointer;" onmouseover="fxf_fn_markContextMenu(true, event);" onmouseout="fxf_fn_markContextMenu(false, event);">';
	var tb=' class="cmtb"';
	var tm=' class="cmtm"';
	var hr='<tr><td id="hr" class="cmhrl"></td><td colspan="5"><hr id="hr" class="cmhrr"></td></tr>\n';
	var o0='cursor:default;opacity:0.15;';
	var o1='cursor:pointer;opacity:1.0;';

	var cmt='<table border="0" cellpadding="2" cellspacing="0">\n';
	cmt += '<tr><td colspan="6" style="border-bottom:1px solid #aaaaaa;background:#dddddd;"><b class="darkgrey">';
	if(oFXP.lang == 1)
	{
		cmt += 'Workflow-Aktionen für ';
		if(ts)
			cmt += 'Aufgabe';
		else
			cmt += 'Projekt';
	}
	else
	{
		cmt += 'Workflow actions for ';
		if(ts)
			cmt += 'task';
		else
			cmt += 'project';
	}
	cmt += ':</b> <b>'+npn+'</b></td></tr>\n';
	cmt += '<tr><td colspan="6"></td></tr>\n';

	// ...Mark/Unmark project
	var mpn=$('pjsmm').value;
	var mk=1;
	if(mpn == pn)
		mk=0;
	id=' id="pjsma_999_'+mk+'_'+pn+'"';
	if(oFXP.lang == 1)
	{
		if(ts)
			tx='Aufgabe';
		else
			tx='Projekt';
		if(mk)
			tx += ' markieren';
		else
			tx += ' demarkieren';
	}
	else
	{
		if(mk)
			tx='Mark ';
		else
			tx='Unmark ';
		if(ts)
			tx += 'task';
		else
			tx += 'project';
	}
	cmt += tr+'<td'+id+' align="center"'+tb+'></td><td'+id+tm+'>&nbsp;'+tx+'&nbsp;</td><td colspan="4"></td></tr>\n';

	cmt += hr;

	// ...Program functions [1=Act. Tasks,2=Plan. + Act. Tasks,3=All Tasks,4=All Act. or Fin. Projects,5=MP,6=Projects,7=All] [Display] [Create] [Edit] [Delete] [4-digit function no.]
	var fl=['100100081','100100034','100100150','','710000209','610000026','710000263','','600100189','711110023','','711110260','701000099','310000043','','411110104','','500100037','610000152','700100138','','710000107','701000110'];
	var al=['sh','ne','ch','de'];
	if(oFXP.lang == 1)
		var at=['Anzeigen','Neuanlage','&Auml;ndern','Löschen', 'Programmfunktion aufrufen im Standardmodus: ','Programmfunktion aufrufen im Modus: '];
	else
		var at=['Display','Create','Edit','Delete', 'Call program function in standard mode: ','Call program function in mode: '];
	var fc=0;
	for(var f=0; f<fl.length; f++)
	{
		if(fl[f].length)
		{
			if(!oFXP.mstructure.length)
				continue;
			var pt=parseInt(fl[f].substr(0,1));
			if(((pt == 1) && ts && (st == 300)) || ((pt == 2) && ts && ((st == 297) || (st == 300))) || ((pt == 3) && ts) || ((pt == 4) && (!st || (st > 299))) || ((pt == 5) && !ts && (pn.length == 4)) || ((pt == 6) && !ts) || (pt == 7))
			{
				var tn=parseInt(fl[f].substr(5));
				tx='';
				for(var m=0; m<oFXP.mstructure.length; m++)
				{
					if(oFXP.mstructure[m].tr == tn)
					{
						tx=oFXP.mstructure[m].txt;
						break;
					}
				}
				if(tx.length)
				{
					var a=fl[f].substr(1,4);
					if(tn == 104)
						var da=2;
					else
					{
						var da=3;
						if(!parseInt(a.substr(2,1)))
						{
							da=2;
							if(!parseInt(a.substr(1,1)))
							{
								da=1;
								if(!parseInt(a.substr(0,1)))
									da=4;
							}
						}
					}
					id=' id="pjsma_'+tn+'_'+da+'_'+pn+'"';
					cmt += tr+'<td'+id+' align="right"'+tb+'><font'+id+' class="s2 lightgrey">'+tn+'</font></td><td'+id+tm+' tooltip="'+at[4]+'<i>'+at[da-1]+'</i>">&nbsp;'+tx+'&nbsp;&nbsp;</td>';
					for(var ac=0; ac<al.length; ac++)
					{
						var ta=0;
						if(parseInt(a.substr(ac,1)))
							ta=ac+1;
						id=' id="pjsma_'+tn+'_'+ta+'_'+pn+'"';
						cmt += '<td'+id+'><div'+id+' style="position:relative;width:17px;height:16px;border:0;padding:0;margin:0;background:url(GFX/ic'+al[ac]+'.png);';
						if(ta)
							cmt += o1+'" tooltip="'+at[5]+'<b>'+at[ac]+'</b>';
						else
							cmt += o0;
						cmt += '">&nbsp;</div></td>';
					}
					cmt += '</tr>\n';
					fc++;
				}
			}
		}
		else
		{
			if(fc > 0)
				cmt += hr;
			fc=0;
		}
	}

	cmt += '</table>\n';
	oID.cmentries.innerHTML=cmt;

	oID.cmenu.style.display='';
	if(mcy+oID.cmenu.clientHeight+24 > dim.sd.pheight)
	{
		if(mcy-oID.cmenu.clientHeight-24 < 0)
			oID.cmenu.style.top=(mcy-Math.floor(oID.cmenu.clientHeight/2))+'px';
		else
			oID.cmenu.style.top=(mcy-oID.cmenu.clientHeight)+'px';
	}
	else
		oID.cmenu.style.top=mcy+'px';
	if(mcx+oID.cmenu.clientWidth+24 > dim.sd.pwidth)
	{
		if(mcx-oID.cmenu.clientWidth-24 < 0)
			oID.cmenu.style.left=(mcx-Math.floor(oID.cmenu.clientWidth/2))+'px';
		else
			oID.cmenu.style.left=(mcx-oID.cmenu.clientWidth)+'px';
	}
	else
		oID.cmenu.style.left=mcx+'px';
	fxf_fn_dispMenu(event,1);
}

function fxf_fn_execPJSMControl(ct)
{
	// Open all projects
	if(ct == 'flop')
		fxf_fn_growProjects(0,'','','!');
	// Close all projects
	else if(ct == 'flcl')
	{
		var pels=$$('[id^="pjsmd_"]');
		if(pels.length)
		{
			var ma=[];
			var mc=0;
			for(var p=0; p<pels.length; p++)
			{
				var mn=pels[p].id.substr(6);
				if(mn.length == 4)
				{
					var sp=$('pjsmfd_'+mn);
					if(sp)
					{
						ma[mc]=mn;
						mc++;
					}
				}
			}
//alert('ma='+ma);

			if(ma.length)
			{
				for(var m=0; m<ma.length; m++)
				{
					var fi=$('pjsmfi_'+ma[m]);
					if(ct == 'flop')
						fi.src=fi.src.substr(0,fi.src.length-6)+'fc.png';
					else
						fi.src=fi.src.substr(0,fi.src.length-6)+'fo.png';
					fxf_fn_togglePJSMFolder(ma[m],1);
				}
			}
		}
	}
	// Update project workflow menu
	else if(ct == 'upd')
		fxf_fn_growProjects(0,'','','');
	// Switch sort order
	else if((ct == 'srt09') || (ct == 'srtaz'))
	{
		if(oSet.sort09 == 'j')
			oSet.sort09='n';
		else
			oSet.sort09='j';
		fxf_fn_growProjects(0,'',oSet.sort09,'');
	}
	// Start/Clear project search
	else
	{
		if(ct == 'cls')
		{
			var e=$('pjsmt_src');
			e.value='';
		}
		fxf_fn_searchPJSM(false);
		fxf_fn_growProjects(0,'','','');
	}
}

function fxf_fn_searchPJSM(focus)
{
	var e=$('pjsmt_src');
	var t=e.value.toLowerCase();
//alert('t='+t);
	if(focus)
	{
		if((t == 'suche') || (t == 'seach'))
			e.value='';
		e.style.color='#000000';
	}
	else
	{
		if((t == 'suche') || (t == 'seach'))
			e.style.color='#a4a4a4';
		else if(!t.length)
		{
			if(oFXP.lang == 1)
				e.value='Suche';
			else
				e.value='Search';
			e.style.color='#a4a4a4';
		}
		else
			e.style.color='#000000';
		$('pjsmt_dummy').focus();
	}

	var sci=$('pjsmc_cls');
	if(sci)
	{
		if(t.length)
			sci.style.display='';
		else
			sci.style.display='none';
	}
}

function fxf_fn_focusNextElement()
{
	if(!fEl)
		return;

	var ne=null;
	if((oFXP.tr == 189) && (fEl.sdir != 0))
	{
		var dm=-1;
		var ei=fEl.id;
		var bp=ei.indexOf('[');
		var id='';
		if(bp > 0)
		{
			id=ei.substr(bp+1,ei.length-bp-2);
			ei=ei.substr(0,bp);
		}
//alert('Focus on next element for: '+fEl.id+' (type='+fEl.type+', sdir='+fEl.sdir+') -- ei='+ei+', id='+id);

		var dm=fxf_fn_getKeyFromColumn(ei);
		if(dm >= 0)
		{
			var pc=fxf_fn_getKeyFromID('P', id);
			if(fEl.kc == 13)
			{
				var p=pc;
				while(true)
				{
					if(fEl.sdir < 0)
					{
						p--;
						if(p < 0)
							p=pArray.length-1;
					}
					else
					{
						p++;
						if(p >= pArray.length)
							p=0;
					}
					if(p == pc)
					{
						ne=fEl;
						break;
					}

					ne=fxf_fn_getField($(ei+'['+pArray[p].uid+']'),true);
					if(ne && (ne.type == fEl.type))
						break;
					else
						ne=null;
				}
			}
			else
			{
				var d=dm;
//alert('Start: d='+d+', mcolumns.length='+mcolumns.length+', fname='+mcolumns[d].fname+', ftype='+mcolumns[d].ftype+', dtype='+mcolumns[d].dtype+' - type='+fEl.type);
				while(true)
				{
					if(fEl.sdir < 0)
					{
						d--;
						if(d < 0)
							d=mcolumns.length-1;
					}
					else
					{
						d++;
						if(d >= mcolumns.length)
							d=0;
					}
					if(d == dm)
					{
						ne=fEl;
						break;
					}
					ne=fxf_fn_getField($(mcolumns[d].fname+'['+id+']'),true);
//alert('d='+d+', mcolumns.length='+mcolumns.length+', fname='+mcolumns[d].fname+', ftype='+mcolumns[d].ftype+', dtype='+mcolumns[d].dtype+' - type='+ne.type);
					if(ne && ((ne.type == 'text') || (ne.type == 'textarea')) && ($('dw_'+mcolumns[d].fname).style.display != 'none') && (pArray[pc].columns[d].style.display != 'none'))
						break;
					else
						ne=null;
				}
			}
		}
		else
		{
			dm=fxf_fn_getKeyFromRColumn(ei);
			if(dm >= 0)
			{
				var pc=fxf_fn_getKeyFromID('R', id);
				if(fEl.kc == 13)
				{
					var p=pc;
					while(true)
					{
						if(fEl.sdir < 0)
						{
							p--;
							if(p < 0)
								p=rArray.length-1;
						}
						else
						{
							p++;
							if(p >= rArray.length)
								p=0;
						}
						if(p == pc)
						{
							ne=fEl;
							break;
						}

						ne=fxf_fn_getField($(ei+'['+rArray[p].uid+']'),true);
						if(ne && (ne.type == fEl.type))
							break;
						else
							ne=null;
					}
				}
				else
				{
					var d=dm;
					while(true)
					{
						if(fEl.sdir < 0)
						{
							d--;
							if(d < 0)
								d=rcolumns.length-1;
						}
						else
						{
							d++;
							if(d >= rcolumns.length)
								d=0;
						}
						if(d == dm)
						{
							ne=fEl;
							break;
						}
						ne=fxf_fn_getField($(rcolumns[d].fname+'['+id+']'),true);
						if(ne && (ne.type == fEl.type))
							break;
						else
							ne=null;
					}
				}
			}
		}
	}

	if(ne)
	{
//alert('Found next element: '+ne.id+' ('+ne.type+')');
		ne.focus();
		ne.select();
	}
	else if(fEl.sdir == 0)
		fxf_fn_unfocusText();

	fEl=null;
}

function fxf_fn_unfocusText()
{
	oID.taconv.style.display='';
	oID.taconv.focus();
	oID.taconv.style.display='none';
	fEl=null;
}

function fxf_fn_getBudgetMinMax(pc,ei)
{
var dt='fxf_fn_getBudgetMinMax(pc='+pc+', ei='+ei+')';
dt=dt+'... no='+pArray[pc].no+', name='+pArray[pc].name;
	var bt=ei.substr(0,3);
	var bs=ei.substr(3);
dt=dt+'\nbt='+bt+', bs='+bs;

	var sub=0.0;
	var add=0.0;

	// Closest parent budget frame
dt=dt+'\n\nClosest parent budget frame:';
	var pmax=-1.0;
	var pr=-1;
	if(pArray[pc].parents.length)
	{
		for(var p=0; p<pArray[pc].parents.length; p++)
		{
			if(pArray[pArray[pc].parents[p]].values[bt+'frame'] > 0.0)
			{
				pr=pArray[pc].parents[p];
				pmax=pArray[pr].values[bt+'frame'];
dt=dt+'\n= PARENT-p='+p+' (='+pr+', no='+pArray[pr].no+', name='+pArray[pr].name+'): -> pmax='+pmax;
				break;
			}
		}
	}
dt=dt+'\n--> pr='+pr+', pmax='+pmax;

	// Maximum frame
dt=dt+'\n\nMaximum frame:';
	var fmax=pmax;
dt=dt+'\n= pmax -> '+fmax;
	if(fmax > 0.0)
	{
		var ac=[pr];
		var lv=0;
		while(ac.length > 0)
		{
dt=dt+'\n* ac='+ac+', lv='+lv;
			var nc=[];
			for(var a=0; a<ac.length; a++)
			{
				for(var c=0; c<pArray[ac[a]].children.length; c++)
				{
					var cc=pArray[ac[a]].children[c];
					if(pArray[cc].type == 'T')	// Task
					{
						sub=pArray[cc].values[bt+'plan'];
						if((bt == 'ehr') || (bt == 'emr'))
							sub=Math.max(sub, pArray[cc].values[bt+'inv']);
						if(sub > 0.0)
						{
							fmax -= sub;
dt=dt+'\n- TASK-CHILDREN-PLAN/INV='+cc+' (no='+pArray[cc].no+', name='+pArray[cc].name+'): -MAX('+bt+'plan/'+bt+'inv)='+sub+' -> fmax='+fmax;
						}
					}
					else if(pArray[cc].type == 'P')	// Subproject
					{
						if(cc != pc)
						{
							sub=pArray[cc].values[bt+'frame'];
							if(sub > 0.0)	// ...with frame
							{
								fmax -= sub;
dt=dt+'\n- SUBPROJECT-CHILDREN-FRAME='+cc+' (no='+pArray[cc].no+', name='+pArray[cc].name+'): -'+bt+'frame='+sub+' -> fmax='+fmax;
							}
							else
								nc[nc.length]=cc;
						}
					}
				}
			}
			ac=nc;
			lv++;
		}
	}
dt=dt+'\n--> fmax='+fmax;

	// Minimum frame
dt=dt+'\n\nMinimum frame:';
	var fmin=pArray[pc].values[bt+'plan'];
	if((bt == 'ehr') || (bt == 'emr'))
		fmin=Math.max(fmin, pArray[pc].values[bt+'inv']);
dt=dt+'\n= MAX planned/invoiced budget -> '+fmin;
	var ac=[pc];
	var lv=0;
	while(ac.length > 0)
	{
dt=dt+'\n* ac='+ac+', lv='+lv;
		var nc=[];
		for(var a=0; a<ac.length; a++)
		{
			for(var c=0; c<pArray[ac[a]].children.length; c++)
			{
				var cc=pArray[ac[a]].children[c];
				if(pArray[cc].type == 'P')	// Subproject
				{
					if(!lv)
					{
						sub=pArray[cc].values[bt+'plan'];
						if((bt == 'ehr') || (bt == 'emr'))
							sub=Math.max(add, pArray[cc].values[bt+'inv']);
						if(sub > 0.0)
						{
							fmin -= sub;
dt=dt+'\n- CHILDREN-PLAN/INV='+cc+' (no='+pArray[cc].no+', name='+pArray[cc].name+'): -MAX('+bt+'plan/'+bt+'inv)='+sub+' -> fmin='+fmin;
						}
					}
					add=pArray[cc].values[bt+'frame'];
					if(add > 0.0)	// ...with frame
					{
						fmin += add;
dt=dt+'\n+ CHILDREN-FRAME='+cc+' (no='+pArray[cc].no+', name='+pArray[cc].name+'): +'+bt+'frame='+add+' -> fmin='+fmin;
					}
					else
						nc[nc.length]=cc;
				}
			}
		}
		ac=nc;
		lv++;
	}
dt=dt+'\n--> fmin='+fmin;

	if(bs == 'frame')
	{
		var bmax=fmax;
		var bmin=fmin;
dt=dt+'\n\nMaximum/Minimum plan:\n--> bmax=fmax='+bmax+'\n--> bmin=fmin='+bmin;
	}
	else
	{
		var bmax=fmax;
dt=dt+'\n\nMaximum plan:\nbmax=fmax='+bmax;
		if(bmax >= 0.0)
		{
			add=pArray[pc].values[bt+'plan'];
			if((bt == 'ehr') || (bt == 'emr'))
				add=Math.max(add, pArray[pc].values[bt+'inv']);
			bmax += add;
dt=dt+'\n+ MAX-PC planned/invoiced budget='+add+' -> bmax='+bmax;
		}
dt=dt+'\n--> bmax=pmax='+bmax;

dt=dt+'\n\nMinimum plan:';
		var bmin=0.0;
		if((bt == 'ehr') || (bt == 'emr'))
		{
			bmin=pArray[pc].values[bt+'inv'];
dt=dt+'\n--> bmin='+bt+'inv='+bmin;
		}
		else
		{
dt=dt+'\n--> bmin='+bmin;
		}
	}
	if((bmax >= 0.0) && (bmin > bmax))
	{
		bmax=0.0;
		bmin=0.0;
dt=dt+'\n--> bmin>bmax: bmax=bmin='+bmax;
	}
//alert(dt);

	return {'pmax':pmax, 'fmax':fmax, 'fmin':fmin, 'bmax':bmax, 'bmin':bmin};
}

function fxf_fn_getBudgetFrameMin(pc,ei,af)
{
var dt='fxf_fn_getBudgetFrameMin(pc='+pc+', ei='+ei+', af='+af+')';
dt=dt+'... no='+pArray[pc].no+', name='+pArray[pc].name;
	var bt=ei.substr(0,3);
	var bs=ei.substr(3);
	var fmin=0.0;
dt=dt+'\nbt='+bt+', bs='+bs+' -> fmin='+fmin;

	if(af)
	{
		fmin=pArray[pc].values[bt+'plan'];
dt=dt+'\n\naf: fmin='+bt+'plan='+fmin;
	}
	if(!fmin)
	{
		if(pArray[pc].children.length)
		{
			for(var p=0; p<pArray[pc].children.length; p++)
			{
				var c=pArray[pc].children[p];
				var add=Math.max(pArray[c].values[ei],pArray[c].values[bt+'plan']);
				fmin += add;
dt=dt+'\n+ CHILDREN='+c+' (no='+pArray[c].no+', name='+pArray[c].name+'): +MAX('+ei+'='+pArray[c].values[ei]+', '+bt+'plan='+pArray[c].values[bt+'plan']+')='+add+' -> fmin='+fmin;
			}
		}
		else
		{
			fmin=pArray[pc].values[bt+'plan'];
dt=dt+'\nNO CHILDREN -> fmin='+bt+'plan='+fmin;
		}
	}

	fmin=Math.max(fmin,0.0);
dt=dt+'\n\n--> fmin='+fmin;
//alert(dt);

	return fmin;
}

function fxf_fn_getBudgetFrameMax(pc,ei,af)
{
var dt='fxf_fn_getBudgetFrameMax(pc='+pc+', ei='+ei+', af='+af+')';
dt=dt+'... no='+pArray[pc].no+', name='+pArray[pc].name;
	var bt=ei.substr(0,3);
	var bs=ei.substr(3);
	var fmax=0.0;
dt=dt+'\nbt='+bt+', bs='+bs+' -> fmax='+fmax;

	if(af)
	{
		fmax=pArray[pc].values[ei];
dt=dt+'\n\naf: fmax='+ei+'='+fmax;
	}
	if(!fmax)
	{
		var minmax=fxf_fn_getBudgetMinMax(pc,ei);
dt=dt + '\n\nminmax: pmax='+minmax.pmax+' - fmax='+minmax.fmax+', fmin='+minmax.fmin+' - bmax='+minmax.bmax+', bmin='+minmax.bmin;
		fmax=minmax.bmax;
dt=dt + '\n-> fmax=minmax.bmax='+fmax;
	}

	fmax=Math.max(fmax,0.0);
dt=dt+'\n\n--> fmax='+fmax;
//alert(dt);

	return fmax;
}

function fxf_fn_checkBudgets(fixit)
{
	var pbe={'err':false, 'bpferr':[], 'bpffix':[], 'bfferr':[], 'bfffix':[]};
	if((pArray.length > 0) && (oSet.cost_see > 0) && (oSet.cost_btype != 'N'))
	{
		// Check projects: planned budget vs. budgetframe
		for(var p=0; p<pArray.length; p++)
		{
			if(pArray[p].type == 'P')
			{
				if(((pArray[p].values.ihrframe > 0.0) && (pArray[p].values.ihrplan > pArray[p].values.ihrframe)) || ((pArray[p].values.ehrframe > 0.0) && (pArray[p].values.ehrplan > pArray[p].values.ehrframe)) || ((pArray[p].values.imrframe > 0.0) && (pArray[p].values.imrplan > pArray[p].values.imrframe)) || ((pArray[p].values.emrframe > 0.0) && (pArray[p].values.emrplan > pArray[p].values.emrframe)))
				{
					if(fixit)
					{
						if(pArray[p].values.ihrframe > 0.0)
							pArray[p].values.ihrframe=Math.max(pArray[p].values.ihrframe,pArray[p].values.ihrplan);
						if(pArray[p].values.ehrframe > 0.0)
							pArray[p].values.ehrframe=Math.max(pArray[p].values.ehrframe,pArray[p].values.ehrplan);
						if(pArray[p].values.imrframe > 0.0)
							pArray[p].values.imrframe=Math.max(pArray[p].values.imrframe,pArray[p].values.imrplan);
						if(pArray[p].values.emrframe > 0.0)
							pArray[p].values.emrframe=Math.max(pArray[p].values.emrframe,pArray[p].values.emrplan);
					}
					else
					{
						pbe.err=true;
						pbe.bpferr[pbe.bpferr.length]=p;
						if(pArray[p].rights & 32)
							pbe.bpffix[pbe.bpffix.length]=p;
					}
				}
			}
		}
		// Check projects: budgetframes
		var ihrfmin=0.0;
		var ehrfmin=0.0;
		var imrfmin=0.0;
		var emrfmin=0.0;
		for(var p=0; p<pArray.length; p++)
		{
			if(pArray[p].type == 'P')
			{
				ihrfmin=fxf_fn_getBudgetFrameMin(p,'ihrframe',false);
				ehrfmin=fxf_fn_getBudgetFrameMin(p,'ehrframe',false);
				imrfmin=fxf_fn_getBudgetFrameMin(p,'imrframe',false);
				emrfmin=fxf_fn_getBudgetFrameMin(p,'emrframe',false);
//alert(p+' - '+pArray[p].no+': ihrfmin='+ihrfmin+', ehrfmin='+ehrfmin+', imrfmin='+imrfmin+', emrfmin='+emrfmin);
				if(((pArray[p].values.ihrframe > 0.0) && (pArray[p].values.ihrframe < ihrfmin)) || ((pArray[p].values.ehrframe > 0.0) && (pArray[p].values.ehrframe < ehrfmin)) || ((pArray[p].values.imrframe > 0.0) && (pArray[p].values.imrframe < imrfmin)) || ((pArray[p].values.emrframe > 0.0) && (pArray[p].values.emrframe < emrfmin)))
				{
					if(fixit)
					{
						if(pArray[p].values.ihrframe > 0.0)
							pArray[p].values.ihrframe=Math.max(pArray[p].values.ihrframe,ihrfmin);
						if(pArray[p].values.ehrframe > 0.0)
							pArray[p].values.ehrframe=Math.max(pArray[p].values.ehrframe,ehrfmin);
						if(pArray[p].values.imrframe > 0.0)
							pArray[p].values.imrframe=Math.max(pArray[p].values.imrframe,imrfmin);
						if(pArray[p].values.emrframe > 0.0)
							pArray[p].values.emrframe=Math.max(pArray[p].values.emrframe,emrfmin);
					}
					else
					{
						pbe.err=true;
						pbe.bfferr[pbe.bfferr.length]=p;
						if(pArray[p].rights & 32)
							pbe.bfffix[pbe.bfffix.length]=p;
					}
				}
			}
		}
	}

//alert('pbe.err='+pbe.err+'\n\npbe.bpferr='+pbe.bpferr+'\npbe.bpffix='+pbe.bpffix+'\n\npbe.bfferr='+pbe.bfferr+'\npbe.bfffix='+pbe.bfffix);
	return pbe;
}

function fxf_fn_getMatrixPCat(pid)
{
	var c=-1;
	var pc=fxf_fn_getKeyFromID('I', pid);
	if(pc >= 0)
		c=pArray[pc].values.pcat;
//alert('pid='+pid+' > pc='+pc+' -> c='+c);

	return c;
}

function fxf_fn_toggleMails()
{
	var id=$('dmseid');
	var pos=id.src.indexOf('/fl_');
	var path=id.src.substr(0,pos+1);
	var typ=id.src.substr(pos+4,2);
//alert('id='+id+'\nid.src='+id.src+'\npos='+pos+'\npath='+path+'\ntyp='+typ);

	if(typ == 'cl')
	{
		id.src=path+'fl_op.png';
		var disp='none';
	}
	else
	{
		id.src=path+'fl_cl.png';
		var disp='';
	}

	var eels=$$('[id^="dmseid_"]');
//alert('eels ['+eels.length+'] =\n'+eels);
	if(eels.length)
	{
		for(var e=0; e<eels.length; e++)
		{
			var tr=eels[e].up('tr');
			tr.style.display=disp;
		}
	}
}

function fxf_fn_resetDesign(design,colors)
{
	if(design)
	{
		oFXP.design='000default';

		// Delete user design style sheet
		var sid=document.getElementById('user_style_sheet');
//alert('Remove design style sheet: sid='+sid);
		if(sid)
		{
			var sp=sid.parentNode;
			sp.removeChild(sid);
		}
	}

	if(colors)
	{
		// Delete menu colors style sheet
		var sid=document.getElementById('menu_colors_sheet');
//alert('Remove menu colors style sheet: sid='+sid);
		if(sid)
		{
			var sp=sid.parentNode;
			sp.removeChild(sid);
		}
	}
}

function fxf_fn_setDesign(d)
{
	var url=fxf_fn_gProgram('design', 'c='+oSet.client+'&p='+oSet.person+'&u='+oSet.user+'&m='+oFXP.mentries+'&d='+fxf_fn_escapeText(d,false)+fxf_fn_gParam());
//alert('url='+url);

	new Ajax.Request(url,
	{
		method:'get', asynchronous:true,
		onSuccess: function(transport)
		{
			var ns=transport.responseText;
//alert('Set Stylesheets:\n\n'+ns);
			fxf_fn_resetDesign(true,true);

			if(ns.length)
			{
				var dm=ns.split('/* Menu colors */');
//alert('Design / Menu Colors:\n\n'+dm);

				// Add user design style sheet
				if((dm.length > 0) && (dm[0].trim().length > 0))
				{
//alert('Design:\n\n'+dm[0]);
					oFXP.design=d;

					var sid=document.createElement('style');
					sid.id='user_style_sheet';
					sid.innerHTML=dm[0].trim();
					document.body.appendChild(sid);
				}

				// Add menu colors style sheet
				if((dm.length > 1) && (dm[1].trim().length > 0))
				{
//alert('Menu colors:\n\n'+dm[1]);
					var sid=document.createElement('style');
					sid.id='menu_colors_sheet';
					sid.innerHTML=dm[1].trim();
					document.body.appendChild(sid);
				}
			}

			// Menu top border
			var bid=fxf_fn_drawMenuBorder('',oFXP.tr);
		}
	});
}

function fxf_fn_prjCheck(s)
{
	var a=s.split('|');
	if(a.length < 12)
		return;

	var auser=parseInt(oSet.user)+'-'+parseInt(oSet.person)+'-'+oFXP.lts;

	var aid=a[0];	// c=CheckOutPerson / r=RequestSender
	var afts=a[1];
	var astatus=parseInt(a[2]);
	var ampid=parseInt(a[3]);
	var apno=a[4];
	var apname=a[5];
	var nuser=a[6]+'-'+a[7]+'-'+a[9];
	var nuname=a[8];
	var nfts=a[10];
	var ntr=parseInt(a[11]);

	if(oFXP.lang == 1)
		var sh='Projektblockade';
	else
		var sh='Project Blockade';
	var sm='';
	if(aid == 'c')
	{
		if(astatus == 1)
		{
			if(oFXP.lang == 1)
			{
				sm='<u class="s5 grey bold">Anfrage zur Aufhebung einer Projektblockade</u><br /><br /><br /><br /><b>'+nuname+'</b><br /><br />bittet um Aufhebung der Blockade des Projekts<br /><br /><b class=blue>'+apno+'</b> <font class=blue>'+apname+'</font><br /><br />welches aktuell von Ihnen bearbeitet wird.<br><br><br><i class=red>(Wird die Blockade sofort aufgehoben, gehen alle ungespeicherten Änderungen verloren!)</i>';
				var b1='Blockade sofort aufheben';
				var b2='Blockade später aufheben';
				var b3='Blockade beibehalten';
			}
			else
			{
				sm='<u class="s5 grey bold">Request for lifting a project blockade</u><br /><br /><br /><br /><b>'+nuname+'</b><br /><br />asks for lifting the blockade of project<br /><br /><b class=blue>'+apno+'</b> <font class=blue>'+apname+'</font><br /><br />which is currently in use by you.<br><br><br><i class=red>(If the blockade is lifted immediately, all unsaved changes will be lost!)</i>';
				var b1='Lift blockade immediately';
				var b2='Lift blockade later';
				var b3='Hold blockade';
			}

			fxf_fn_waitScreen('');
			fxf_fn_question(sh, sm, [b1,b2,b3], ['fxf_fn_keepUserAlive(\''+ampid+'|2\');fxf_fn_resetChanged();fxf_fn_waitScreen(\'lifting\');fxf_fn_loadTR.delay('+oFXP.ldelay+','+oFXP.tr+',\'lift\');', 'fxf_fn_keepUserAlive(\''+ampid+'|3\');', 'fxf_fn_keepUserAlive(\''+ampid+'|4\');'], 220);
		}
		else if(oID.iacont.style.display != 'none')
		{
			oID.iacont.style.display='none';
			oID.iact.style.display='none';
		}
	}
	else if(aid == 'r')
	{
		if(astatus > 1)
		{
			if(astatus == 2)
			{
				if(oFXP.lang == 1)
					sm='<u class="s5 grey bold">Anfrage akzeptiert</u><br /><br /><br /><br /><b>'+nuname+'</b><br /><br /> hat Ihre Anfrage zur Aufhebung der Blockade des Projekts<br /><br /><b class=blue>'+apno+'</b> <font class=blue>'+apname+'</font><br /><br />sofort akzeptiert.';
				else
					sm='<u class="s5 grey bold">Request accepted</u><br /><br /><br /><br /><b>'+nuname+'</b><br /><br /> has accepted your request to lift the blockade of project<br /><br /><b class=blue>'+apno+'</b> <font class=blue>'+apname+'</font><br /><br />immediately.';
			}
			else if(astatus == 3)
			{
				if(oFXP.lang == 1)
					sm='<u class="s5 grey bold">Anfrage zur Kenntnis genommen</u><br /><br /><br /><br /><b>'+nuname+'</b><br /><br /> hat Ihre Anfrage zur Aufhebung der Blockade des Projekts<br /><br /><b class=blue>'+apno+'</b> <font class=blue>'+apname+'</font><br /><br />zur Kenntnis genommen und wird die Blockade in Kürze aufheben.';
				else
					sm='<u class="s5 grey bold">Request acknowledged</u><br /><br /><br /><br /><b>'+nuname+'</b><br /><br /> has acknowledged your request to lift the blockade of project<br /><br /><b class=blue>'+apno+'</b> <font class=blue>'+apname+'</font><br /><br />and will lift the blockade in a moment.';
			}
			else if(astatus == 4)
			{
				if(oFXP.lang == 1)
					sm='<u class="s5 grey bold">Anfrage abgewiesen</u><br /><br /><br /><br /><b>'+nuname+'</b><br /><br /> hat Ihre Anfrage zur Aufhebung der Blockade des Projekts<br /><br /><b class=blue>'+apno+'</b> <font class=blue>'+apname+'</font><br /><br />leider abgewiesen.';
				else
					sm='<u class="s5 grey bold">Request rejected</u><br /><br /><br /><br /><b>'+nuname+'</b><br /><br /> has unfortunately rejected your request to lift the blockade of project<br /><br /><b class=blue>'+apno+'</b> <font class=blue>'+apname+'</font>';
			}
			else if(astatus == 98)
			{
				if(oFXP.lang == 1)
					sm='<u class="s5 grey bold">Person abgemeldet</u><br /><br /><br /><br /><b>'+nuname+'</b><br /><br /> hat sich von fx-project abgemeldet. Die Blockade des Projekts<br /><br /><b class=blue>'+apno+'</b> <font class=blue>'+apname+'</font><br /><br />wurde dadurch aufgehoben.';
				else
					sm='<u class="s5 grey bold">Person logged out</u><br /><br /><br /><br /><b>'+nuname+'</b><br /><br /> has logged out of fx-project. The blockade of project<br /><br /><b class=blue>'+apno+'</b> <font class=blue>'+apname+'</font><br /><br />was thereby lifted.';
			}
			else if(oFXP.lang == 1)
				sm='<u class="s5 grey bold">Blockade aufgehoben</u><br /><br /><br /><br /><b>'+nuname+'</b><br /><br /> hat die Blockade des Projekts<br /><br /><b class=blue>'+apno+'</b> <font class=blue>'+apname+'</font><br /><br /> aufgehoben.';
			else
				sm='<u class="s5 grey bold">Blockade lifted</u><br /><br /><br /><br /><b>'+nuname+'</b><br /><br /> has lifted the blockade of project<br /><br /><b class=blue>'+apno+'</b> <font class=blue>'+apname+'</font>';

			fxf_fn_waitScreen('');
			fxf_fn_question(sh, sm, ['OK'], [''], 100);
		}

		if(oFXP.tr == 281)
		{
			var psf=$('pci_st');
			if(psf)
			{
				ps=psf.innerHTML.split('|');
				if(astatus < 98)
					sm=ps[astatus+2];
				else if(astatus == 98)
					sm=ps[7];
				else
					sm=ps[8];

				var sfid=$('pci_smp_'+ampid);
				if(sfid)
					sfid.innerHTML='<font class="lightgrey">'+nfts.substr(8,2)+':'+nfts.substr(10,2)+'</font> '+sm;

				var afid=$('pci_amp_'+ampid);
				if(afid)
				{
					if(astatus == 0)
						afid.innerHTML='<span class="btn" onclick="fxf_fn_keepUserAlive(\''+ampid+'|1\');">'+ps[1]+'</span>';
					else if(astatus < 98)
						afid.innerHTML='<img src="GFX/uploading.gif" tooltip="'+ps[0]+'&hellip;">';
					else
						afid.innerHTML='';
				}
			}
		}
	}
}

function fxf_fn_clock(tt,sync)
{
	if(oFXP.lang > 0)
	{
		var df=fxf_fn_getDateformat();

		var adate=new Date();

		var add=adate.getDate();
		var adm=adate.getMonth()+1;
		var ady=adate.getFullYear();

		var adh=adate.getHours();
		var adi=adate.getMinutes();
		var ads=adate.getSeconds();

		var awd=adate.getDay();
		var twd=fxf_fn_getText(awd+29);
		if(twd.length)
			twd=twd.substr(0,2)+',&nbsp;';

		var ts=fxf_fn_addLeadingZeros(ady,4)+fxf_fn_addLeadingZeros(adm,2)+fxf_fn_addLeadingZeros(add,2);
		var dt=fxf_fn_convertTimestamp2Date(ts);

		oID.fxtaskc.innerHTML=twd+dt.date+'&nbsp;&nbsp;'+fxf_fn_addLeadingZeros(adh,2)+':'+fxf_fn_addLeadingZeros(adi,2);

		// Synchronization info?
		if(oID.fxtaskc.attributes && oID.fxtaskc.attributes['tooltip'] && (tt || (oID.fxtaskc.attributes['tooltip'].value == '')))
			fxf_fn_syncClock();
	}

	if(sync)
		fxf_fn_clock.delay(60-ads, false,true);
}

function fxf_fn_syncClock()
{
	var url=fxf_fn_gProgram('timestamp', fxf_fn_gParam());
//alert(url);
	new Ajax.Request(url,
	{
		method:'get', asynchronous:true,
		onSuccess: function(transport)
		{
			var rt=transport.responseText;

			// ...Client Unix-Timestamp
			var cuts=parseInt(rt);
			// ...Server Unix-Timestamp
			var d=new Date();
			var suts=Math.floor(d.getTime()/1000);

			// ...Difference
			uts_diff=suts-cuts;
//alert('rt='+rt+'\n\ncuts='+cuts+'\nsuts='+suts+'\n\n-> uts_diff='+uts_diff);

			// ...Tooltip
			var ttl=oFXP.lang;
			var llang=$('log_lang');
			if(llang)
				ttl=parseInt(ttlang.innerHTML);
			if(ttl == 1)
				var uts_tt='Zeitdifferenz zum Server';
			else
				var uts_tt='Time difference to server';
			uts_tt += ':<br /><b>';
			if(uts_diff >= 0)
				uts_tt=uts_tt+'+';
			uts_tt=uts_tt+uts_diff+'s</b>';
			uts_tt=uts_tt+'&nbsp;&nbsp;<i>(';
			if(uts_diff >= 0)
				uts_tt=uts_tt+'+';
			uts_tt=uts_tt+fxf_fn_sec2time(uts_diff,false,true)+')</i>';
			oID.fxtaskc.attributes['tooltip'].value=uts_tt;
		}
	});
}

function fxf_fn_bParamMinMax(pc)
{
	var pr=-1;
	var ihrmin=pArray[pc].values.ihrused;
	var ihrmax=0.0;
	var ehrmin=pArray[pc].values.ehrinv;
	var ehrmax=0.0;
	if(pArray[pc].parents.length)
	{
		for(p=0; p<pArray[pc].parents.length; p++)
		{
			if(pArray[pArray[pc].parents[p]].values.ihrframe > ihrmax)
			{
				pr=pArray[pc].parents[p];
				ihrmax=pArray[pc].values.ihrplan+pArray[pr].values.ihrframe-pArray[pr].values.ihrplan;
				break;
			}
		}
		for(p=0; p<pArray[pc].parents.length; p++)
		{
			if(pArray[pArray[pc].parents[p]].values.ehrframe > ehrmax)
			{
				pr=pArray[pc].parents[p];
				ehrmax=pArray[pc].values.ehrplan+pArray[pr].values.ehrframe-pArray[pr].values.ehrplan;
				break;
			}
		}
	}

	var bp='&po='+pc+'&pr='+pArray[pc].id+'&no='+pArray[pc].no+' '+fxf_fn_escapeText(pArray[pc].name,false)+'&cp='+pArray[pc].values.ccp+'&sk='+pArray[pc].values.skill+'&hr='+pArray[pc].values.hr+'&pe='+pArray[pc].values.pe+'&pc='+pArray[pc].values.pc+'&pp='+pArray[pc].values.pp+'&sp='+pArray[pc].values.sp+'&ef='+pArray[pc].values.peffort+'&ib='+pArray[pc].values.ihrplan+'&eb='+pArray[pc].values.ehrplan+'&in='+ihrmin+'&ix='+ihrmax+'&en='+ehrmin+'&ex='+ehrmax+fxf_fn_gParam();
//alert('bp='+bp);

	return bp;
}

function fxf_fn_PPSPPopup(href,pc,popup)
{
//alert('fxf_fn_PPSPPopup:\nhref='+href+'\npc='+pc+'\npopup='+popup);
	if(actEvent)
	{
		fxf_fn_PPSPPopup.delay(oFXP.sdelay, href,pc,popup);
		return;
	}
fxf_fn_writeDebug('log+', '<b class="red">fxf_fn_PPSPPopup</b> actEvent='+actEvent);

	fxf_fn_waitScreen('calc');
	if(!href.length)
	{
		tSet.hrppspcheck='';
		var href=fxf_fn_gProgram('popup_ppsp', 'md=0&sf=1'+fxf_fn_bParamMinMax(pc));
//alert('href='+href);
	}

	var pop=false;
	if(popup > 0)
	{
		fxf_fn_fxLink(false,href);
		pop=true;
	}
	else
	{
		var url=href;
		var qi=url.indexOf('?');
		if(qi < 0)
			url += '?';
		else
			url += '&';
		url += 'no_rdb=1&lng='+oFXP.lang+fxf_fn_gParam();
		var pst='tr='+oFXP.tr+'&hp=1';
		new Ajax.Request(url,
		{
			method:'post', postBody:pst, asynchronous:asajax,
			onSuccess: function(transport)
			{
				var ret=transport.responseText;
				if(ret.substr(0,6) == '<input')
				{
					oID.iainfo.innerHTML=ret;
					fxf_fn_ppspClose(false);
					fxf_fn_waitScreen('');
				}
				else
				{
					popup=true;
					fxf_fn_fxLinkDisplay(ret);
				}
			}
		});
	}

	return pop;
}

function fxf_fn_checkUnit()
{
	// Only allow unit change for certain program functions: 23=Projects (Detail), 152=Multi-Report, 189=Projects (Matrix) + 209=Project Overview
	if((oFXP.tr == 23) || (oFXP.tr == 152) || (oFXP.tr == 189) || (oFXP.tr == 209))
	{
		if(oSet.cunit)
			gSet.dunit=oSet.cunit;
		else if(oSet.punit)
			gSet.dunit=oSet.punit;
		else
			gSet.dunit=2;	// Hrs.

		oID.fxtasku.innerHTML=oSet.unitlit[gSet.dunit];
		if(!oSet.cunit)
		{
			oID.fxtasku.setOpacity(1.0);
			gSet.cunit=true;
		}
		else
		{
			oID.fxtasku.setOpacity(0.50);
			gSet.cunit=false;
		}
	}
	else
	{
		gSet.dunit=2;	// Hrs.
		oID.fxtasku.innerHTML=oSet.unitlit[gSet.dunit];
		oID.fxtasku.setOpacity(0.50);
		gSet.cunit=false;
	}
	oID.fxtasku.style.display='';
	oID.fxtaskup.style.display='none';
}

function fxf_fn_setUnit(ounit)
{
//alert('Adjust fields for new unit: '+gSet.dunit+' = '+oSet.unitlit[gSet.dunit]+'\nounit='+ounit);
	var frs='';
	var frm='';
	var lrs='';
	if(oFXP.tr == 23)	// Projects (Detail)
	{
		var ouf=$$('[id^="Aufwand_Soll"]');
		if(ouf.length)
		{
			frm=ouf[0].attributes['fxform'].value;
			frs += fxf_fn_drawUnits(Array('Aufwand_Soll|Zeiteinheit_Aufw_S','Max_Zeit_Aufw|Zeiteinheit_max_Zeitaufw','Soll_Verzoeg|Zeiteinheit_Soll_Verz','Aufwand_Ist|Zeiteinheit_Aufw_I'), ounit);
		}
		frs += fxf_fn_drawListUnits(Array('eff_|unit_eff_'), ounit);
	}
	else if((oFXP.tr == 152) || (oFXP.tr == 209))	// Project Status Report (Internal) + Project Overview
	{
		lrs += fxf_fn_drawListUnits(Array('eff_|unit_eff_'), ounit);
	}
	else if(oFXP.tr == 189)	// Projects (Matrix)
	{
		fxf_fn_drawDetail(Array('peffort','aeffort','meffort'));
//		fxf_fn_drawHRData(Array('rpc'));
	}
	else
	{
		var ouf=$('Aufwand_Soll');
		if(!ouf)
			ouf=$('Zeiteinheit_Aufw_S');
		if(ouf)
		{
			frm=ouf.attributes['fxform'].value;
			frs += fxf_fn_drawUnits(Array('Aufwand_Soll|Zeiteinheit_Aufw_S','Max_Zeit_Aufw|Zeiteinheit_max_Zeitaufw','Soll_Verzoeg|Zeiteinheit_Soll_Verz','Aufwand_Ist|Zeiteinheit_Aufw_I'), ounit);
		}
		frs += fxf_fn_drawListUnits(Array('eff_|unit_eff_'), ounit);
	}

	// Save changed fields
	if(frs.length && frm.length)
	{
//alert('Save changed fields: frm='+frm+'\n'+frs);
		fxf_fn_saveFields.delay(oFXP.ldelay, frs,'frm='+frm);
	}
	// Save changed list entries
	if(lrs.length)
	{
//alert('Save changed list entries:\n'+lrs);
		fxf_fn_saveListEntries.delay(oFXP.ldelay, lrs);
	}
}

function fxf_fn_drawUnits(fa, ounit)
{
	var frs='';
	if(fa && fa.length)
	{
		for(var f=0; f<fa.length; f++)
		{
			var pp=fa[f].indexOf('|');
			if(pp)
			{
				var fv=fa[f].substr(0,pp);
				var fu=fa[f].substr(pp+1);
			}
			else
			{
				var fv=fa[f];
				var fu='';
			}
//alert('fv='+fv+', fu='+fu+' -- ounit='+ounit+' ('+oSet.unitlit[ounit]+') -> dunit='+gSet.dunit+' ('+oSet.unitlit[gSet.dunit]+')');

			// Adjust values
			var fels=$$('[id^="td_'+fv+'"]');
			if(fels.length)
			{
//alert(f+': id^=td_'+fv+'... found '+fels.length+' elements.');
				for(var i=0; i<fels.length; i++)
				{
					var id=fels[i].id.substr(3);
					var ifv=$(id);
					if(ifv && (ifv.type == 'text'))
					{
						ifv=fxf_fn_getField(ifv,true);
						if(!(id in sunit))
						{
							var ov=fxf_fn_string2float(ifv.value);
							sv=parseInt(ov*oSet.unitcalc[ounit]);
							sunit[id]=sv;
						}
						if(id in sunit)
						{
							var nv=fxf_fn_float2string(Math.round(sunit[id]*100.0/oSet.unitcalc[gSet.dunit])/100.0);
							ifv.value=nv;
							ifv.fxv=ifv.value;
							if(frs.length)
								frs += '&';
							frs += id+'=tx'+ifv.value;
						}
					}
					else
					{
						var otd=fels[i].innerHTML;
						var otv=otd.replace(/(<([^>]+)>)/ig,'').trim();
						if(!(id in sunit) && (otv.substr(0,1) != '['))
						{
							var ov=fxf_fn_string2float(otv);
							var sv=parseInt(ov*oSet.unitcalc[ounit]);
							sunit[id]=sv;
						}
						if(id in sunit)
						{
							var nv=fxf_fn_float2string(Math.round(sunit[id]*100.0/oSet.unitcalc[gSet.dunit])/100.0);
							var ntd=otd.replace(new RegExp(otv, 'g'), nv);
							fels[i].innerHTML=ntd;
						}
					}
				}
			}

			// Adjust units
			if(fu.length)
			{
				var fels=$$('[id^="td_'+fu+'"]');
				if(fels.length)
				{
//alert(f+': id^=td_'+fu+'... found '+fels.length+' elements.');
					for(var i=0; i<fels.length; i++)
					{
						var id=fels[i].id.substr(3);
						var ifu=fxf_fn_getField($(id),true);
						var ict=true;
						if(ifu && ifu.attributes && ifu.attributes['value'])
						{
							if(ifu.type && (ifu.type != 'hidden'))
								ict=false;
							ifu.value=oSet.unitselect[gSet.dunit];
							ifu.fxv=ifu.value;
							if(frs.length)
								frs += '&';
							frs += id+'=in'+ifu.value;
						}
						if(ict)
						{
							var otd=fels[i].innerHTML;
							var ntd=otd.replace(new RegExp(oSet.unitlit[ounit], 'g'), oSet.unitlit[gSet.dunit]);
							fels[i].innerHTML=ntd;
						}
					}
				}
			}
		}
	}

	return frs;
}

function fxf_fn_drawListUnits(fa, ounit)
{
	var lrs='';
	if(fa && fa.length)
	{
		var dm=$('icon_dmanager');

		for(var f=0; f<fa.length; f++)
		{
			var pp=fa[f].indexOf('|');
			if(pp)
			{
				var fv=fa[f].substr(0,pp);
				var fu=fa[f].substr(pp+1);
			}
			else
			{
				var fv=fa[f];
				var fu='';
			}

			// Adjust values
			var fels=$$('[id^="'+fv+'"]');
			if(fels.length)
			{
//alert(f+': fv='+fv+'... found '+fels.length+' elements.');
				for(var i=0; i<fels.length; i++)
				{
					if(!(fels[i].id in sunit))
					{
						var ov=fxf_fn_string2float(fels[i].innerHTML);
						var sv=parseInt(ov*oSet.unitcalc[ounit]);
						sunit[fels[i].id]=sv;
					}
					if(fels[i].id in sunit)
					{
						var nv=fxf_fn_float2string(Math.round(sunit[fels[i].id]*100.0/oSet.unitcalc[gSet.dunit])/100.0);
						fels[i].innerHTML=nv;
						if(dm)
						{
							if(lrs.length)
								lrs += '&';
							lrs += fels[i].id+'='+nv;
						}
					}
				}
			}

			// Adjust units
			if(fu.length)
			{
				var uels=$$('[id^="'+fu+'"]');
				if(uels.length)
				{
//alert(f+': fu='+fu+'... found '+uels.length+' elements.');
					for(var i=0; i<uels.length; i++)
					{
						uels[i].innerHTML=oSet.unitlit[gSet.dunit];
						if(dm)
						{
							if(lrs.length)
								lrs += '&';
							lrs += uels[i].id+'='+oSet.unitlit[gSet.dunit];
						}
					}
				}
			}
		}
	}

	return lrs;
}

function fxf_fn_print(did, title, event)
{
	var pmadd='';

	if((title == undefined) || !title.length)
		title='fx-project Print '+oFXP.tr;

	var d=null;
	if(did.length)
		d=$(did);
	if(!d)
		d=oID.fxwork;

	var full=false;
	if((did == 'gantt') || (oFXP.tr == 209))
		full=true;

	if(oFXP.tr == 209)	// Project Overview
		var c=d.outerHTML;
	else
		var c=d.innerHTML;
	c=c.replace(/\ id\=\"/g, ' id="prt_');	//.replace(/\:absolute/ig, ':relative');
//alert('fxf_fn_print:\n\ndid='+did+'\ntitle='+title+'\nfull='+full+'\n\n'+c);

	if(oFXP.tr == 189)	// Matrix
	{
		if(did == 'gantt')
		{
			var dd=2*(1+fxf_fn_diffDays(sdate,pArray[0].sdate))+1+fxf_fn_diffDays(pArray[0].sdate,pArray[0].edate);
			var ww=1+Math.max(Math.ceil(640/zdays[oFXP.zoom])*zdays[oFXP.zoom],dd*zdays[oFXP.zoom]);
			var wh=1+Math.max(10*dh,oID.gtime.clientHeight+(pArray.length+3)*dh);
//alert('Lines: pArray.length='+pArray.length+', mlines='+mlines+', dh='+dh+', gtime.height='+oID.gtime.clientHeight+'\nColumns: sdate='+sdate+', edate='+edate+', dd='+dd+', mdays='+mdays+', oFXP.zoom='+oFXP.zoom+', zdays[oFXP.zoom]='+zdays[oFXP.zoom]+'\n\nww='+ww+', wh='+wh);
		}
		else
		{
			var ww=d.clientWidth-11;
			var wh=d.clientHeight;
		}
	}
	else if(oFXP.tr == 209)	// Project Overview
	{
		var p=fxf_fn_getBCR($('podet'));
		var ww=p.width+18;
		var wh=oID.fxwork.scrollHeight-p.top+dim.wd.top+18;
		pmadd='padding-left:6px;';
	}
	else
	{
		var p=fxf_fn_getBCR(d);
		var pd=$('fxp_prt_data');
		if(!pd)
			pd=$('fxp_prt_div');
		if(pd)
		{
			full=true;
			p=fxf_fn_getBCR(pd);
			if(pd.style)
			{
//alert('style: width='+pd.style.width+', height='+pd.style.height+' - scrollWidth='+pd.scrollWidth+', scrollHeight='+pd.scrollHeight+' -- left='+pd.style.left+', top='+pd.style.top+', right='+pd.style.right+', bottom='+pd.style.bottom);
				if(pd.scrollWidth)
					p.width=parseInt(pd.scrollWidth);
				else if(pd.style.width)
					p.width=parseInt(pd.style.width);
				if(pd.style.left)
					p.width += parseInt(pd.style.left);
				if(pd.style.right)
					p.width += parseInt(pd.style.right);
				p.width += 16;

				if(pd.scrollHeight)
					p.height=parseInt(pd.scrollHeight);
				else if(pd.style.height)
					p.height=parseInt(pd.style.height);
				if(pd.style.top)
					p.height += parseInt(pd.style.top);
				if(pd.style.bottom)
					p.height += parseInt(pd.style.bottom);
				p.height += 16;
			}
		}
		var ww=p.width;
		var wh=p.height;
	}
//alert('ww='+ww+', wh='+wh);

	var dc='<!DOCTYPE HTML PUBLIC "-/'+'/W3C/'+'/DTD HTML 4.01/'+'/EN" "https:/'+'/www.w3.org/TR/html4/strict.dtd">\n';
	dc += '<html>\n';
	dc += '\t<head>\n';
	dc += '\t\t<title>'+title.replace(/\<.*?\>/gi, '')+'</title>\n';
	dc += '\t\t<meta http-equiv="content-type" content="text/html; charset=UTF-8">\n';
	dc += '\t\t<link rel="stylesheet" type="text/css" href="./CSS/00default.css">\n';
	var sid=document.getElementById('user_style_sheet');
	if(sid)
	{
		dc += '\t\t<style type="text/css">\n';
		dc += sid.innerHTML;
		dc += '\t\t</style>\n';
	}
	dc += '\t</head>\n';
	dc += '\t<body style="margin:0;padding:0;background:#ffffff;">\n';
	if(full)
		dc += '\t\t<div id="prt_body" style="position:absolute;left:0;top:0;right:0;bottom:0;">\n';
	else
		dc += '\t\t<div id="prt_body" style="position:relative;"><br />\n';
	if(did.length)
	{
		dc += '\t\t\t<div id="prt_headerc" style="position:relative;left:0;top:0;right:0;height:36px;border:0;border-top-left-radius:8px;border-top-right-radius:8px;overflow:hidden;">\n';
		dc += '\t\t\t\t'+oID.fxheader.outerHTML+'\n';
		dc += '\t\t\t</div>\n';
	}
	if(full)
		dc += '\t\t\t<div id="prt_main" style="position:absolute;left:0;top:36px;right:0;bottom:13px;'+pmadd+'">\n';
	else
		dc += '\t\t\t<div id="prt_main" style="position:relative;left:0;top:0;width:auto;height:auto;'+pmadd+'">\n';
	if((oFXP.tr == 189) && (did == 'gantt'))
	{
		var a=$('structure');
		var i=a.innerHTML;
		i=i.replace(/\ id\=\"/g, ' id="prt_');
		dc += '<div id="prt_structure" class="mborder" style="position:absolute;left:0;top:0;width:'+a.clientWidth+'px;bottom:0;margin:0;padding:0;">\n';
		dc += i;
		dc += '</div>\n';

		dc += '<div id="prt_gantt" style="position:absolute;left:'+(a.clientWidth+1)+'px;top:0;width:'+ww+'px;bottom:0;margin:0;padding:0;">\n';
		dc += c;
		dc += '</div>\n';

		ww += a.clientWidth;
	}
	else
		dc += c;
	dc += '\t\t\t</div>\n';
	if(did.length)
	{
		if(full)
			dc += '\t\t<div id="prt_footerc" style="position:absolute;left:0;right:0;bottom:0;height:13px;border:0;border-bottom-left-radius:8px;border-bottom-right-radius:8px;background:#acacac;">\n';
		else
			dc += '\t\t<div id="prt_footerc" style="position:relative;left:0;top:2px;right:0;height:13px;border:0;border-bottom-left-radius:8px;border-bottom-right-radius:8px;background:#acacac;">\n';
		dc += '\t\t\t<div id="prt_footer" style="position:absolute;left:0;top:-2px;right:0;bottom:0;padding:0 12px;"><span style="float:right;font-size:11px;color:#ffffff;">'+'P'+'r'+'int'+'ed'+' '+'wi'+'th'+' '+'f'+'x'+'-'+'p'+'r'+'o'+'j'+'e'+'c'+'t'+'</span></div>\n';
		dc += '\t\t</div>\n';
	}
	dc += '\t\t</div>\n';
	dc += '\t</body>\n';
	dc += '</html>';

	var idw=window.open('', '_blank', 'top=0, left=0, width='+ww+', height='+(wh+50));
	idw.document.write(dc);

	if(oFXP.tr == 189)	// Matrix
	{
		if(did == 'gantt')
		{
			var id=idw.document.getElementById('prt_siright');
			if(id)
				id.style.display='none';

			var id=idw.document.getElementById('prt_sheaders');
			if(id)
				id.style.display='none';

			var id=idw.document.getElementById('prt_smain');
			if(id)
			{
				id.style.overflow='hidden';
				id.scrollTop=0;
				id.scrollLeft=0;
			}

			var id=idw.document.getElementById('prt_gtime');
			if(id)
				id.style.width='100%';

			var id=idw.document.getElementById('prt_gileft');
			if(id)
				id.style.display='none';

			var id=idw.document.getElementById('prt_giright');
			if(id)
				id.style.display='none';

			var id=idw.document.getElementById('prt_gthide');
			if(id)
				id.style.display='none';

			var id=idw.document.getElementById('prt_gmain');
			if(id)
			{
				id.style.overflow='hidden';
				id.scrollTop=0;
				id.scrollLeft=0;
			}
		}
		else
		{
			var id=idw.document.getElementById('prt_main');
			if(id)
			{
				id.style.width=ww+'px';
				id.style.height=wh+'px';
			}

			var id=idw.document.getElementById('prt_actionbar');
			if(id)
				id.style.display='none';

			var id=idw.document.getElementById('prt_sdg_message');
			if(id)
				id.style.display='none';

			var id=idw.document.getElementById('prt_sdg_container');
			if(id)
			{
				id.style.left='0';
				id.style.right='0';
				id.style.top='0';

				// Assign scroll positions
				var id=idw.document.getElementById('prt_smain');
				if(id)
				{
					id.scrollTop=oID.smain.scrollTop;
					id.scrollLeft=oID.smain.scrollLeft;
				}
				var id=idw.document.getElementById('prt_dmain');
				if(id)
				{
					id.scrollTop=oID.dmain.scrollTop;
					id.scrollLeft=oID.dmain.scrollLeft;
				}
				var id=idw.document.getElementById('prt_gmain');
				if(id)
				{
					id.scrollTop=oID.gmain.scrollTop;
					id.scrollLeft=oID.gmain.scrollLeft;
				}
			}

			var id=idw.document.getElementById('prt_ndw_container');
			if(id)
			{
				id.style.left='0';
				id.style.right='0';

				// Assign scroll positions
				var id=idw.document.getElementById('prt_nmain');
				if(id)
				{
					id.scrollTop=oID.nmain.scrollTop;
					id.scrollLeft=oID.nmain.scrollLeft;
				}
				var id=idw.document.getElementById('prt_amain');
				if(id)
				{
					id.scrollTop=oID.amain.scrollTop;
					id.scrollLeft=oID.amain.scrollLeft;
				}
				var id=idw.document.getElementById('prt_wmain');
				if(id)
				{
					id.scrollTop=oID.wmain.scrollTop;
					id.scrollLeft=oID.wmain.scrollLeft;
				}
			}
		}

		var id=idw.document.getElementById('prt_overview');
		if(id)
			id.style.display='none';
	}

	if(event != undefined)
	{
		Event.stop(event);
	}

	idw.document.close();
	idw.focus();
	idw.print();
	idw.close();
}

function fxf_fn_closeHRPool()
{
	if(!oID.ndw_container || !oID.hrpool)
		return;

	oID.hrpool.style.display='none';
}

function fxf_fn_HRHeight()
{
	if(!oID.ndw_container || !oID.hrpool)
		return;

	if(hlarge)
	{
		var hheight=0;
		for(var r=0; r<aArray.length; r++)
		{
			if(aArray[r].warea)
				hheight += dh;
		}

		var nrpool=$('nrpool');
		if((pAct < 0) || !pArray[pAct] || (pArray[pAct].type != 'T'))
			nrpool.style.display='none';
		else
		{
			nrpool.style.display='';
			nrpool.style.top=(hheight+1)+'px';
		}

		hheight=Math.max(hheight,oID.wmain.clientHeight);

		oID.nwork.style.height=hheight+'px';
		oID.awork.style.height=hheight+'px';
		oID.wbars.style.height=hheight+'px';
	}

	if(oID.hrpool.style.display != 'none')
		fxf_fn_openHRPool()
}

function fxf_fn_HREnlarge(rst)
{
	if(!rst || (rst && tSet.hrrestore))
	{
		if(hrheight && !hlarge)
		{
			$('hrenlarget').style.display='none';
			$('hrshrinkt').style.display='';
			if(!slarge)
			{
				slarge=true;
				fxf_fn_showArea(1,false);
			}
			if(!dlarge)
			{
				dlarge=true;
				fxf_fn_showArea(2,false);
			}
			if(!glarge)
			{
				glarge=true;
				fxf_fn_showArea(3,false);
			}
			hlarge=true;
			fxf_fn_drawAll();

			if(rst)
				tSet.hrrestore=false;
		}
	}
}

function fxf_fn_HRShrink(rst)
{
	if(!rst || (rst && !tSet.hrrestore))
	{
		if(hrheight && hlarge)
		{
			$('hrenlarget').style.display='';
			$('hrshrinkt').style.display='none';
			hlarge=false;
			if(oID.ndw_container)
				hrheight=parseInt(oID.ndw_container.style.height);
			fxf_fn_drawHRPlanning();
			fxf_fn_resize();
			if(rst)
				tSet.hrrestore=true;
		}
	}
}

function fxf_fn_hrCheckDT(rp,srtmd,srtid,srttp)
{
	var rc='';
	if(rArray && rArray[rp])
	{
		if(srtmd == 2)		// Department
		{
			if(srtid < 0)
				rc='=';
			else
			{
				var fnd=-1;
				for(var c=0; c<rArray[rp].deps.length; c++)
				{
					if((parseInt(rArray[rp].deps[c].substr(1)) == srtid) && (!srttp.length || (rArray[rp].deps[c].substr(0,1) == srttp)))
					{
						fnd=c;
						break;
					}
				}
				if(fnd >= 0)
					rc=rArray[rp].deps[fnd].substr(0,1);
			}
		}
		else if(srtmd == 3)	// Team
		{
			if(srtid < 0)
				rc='=';
			else
			{
				var fnd=-1;
				for(var c=0; c<rArray[rp].teams.length; c++)
				{
					if((parseInt(rArray[rp].teams[c].substr(1)) == srtid) && (!srttp.length || (rArray[rp].teams[c].substr(0,1) == srttp)))
					{
						fnd=c;
						break;
					}
				}
				if(fnd >= 0)
					rc=rArray[rp].teams[fnd].substr(0,1);
			}
		}
		else
			rc='=';
	}
	return rc;
}

function fxf_fn_hrToggleSkills(oc)
{
	if(oc)
	{
		$('hrsko').style.display='none';
		$('hrskc').style.display='';
	}
	else
	{
		$('hrsko').style.display='';
		$('hrskc').style.display='none';
	}

	var els=$$('[id$="_sk"]');
	if(els.length)
	{
		for(var e=0; e<els.length; e++)
		{
			if(els[e].id.substr(0,4) == 'rptd')
			{
				var rid=parseInt(els[e].id.substr(4,els[e].id.length-7));
				var sko=$('hrsko'+rid);
				if(sko)
				{
					var skc=$('hrskc'+rid);
					if(oc)
					{
						els[e].style.height='100%';
						sko.style.display='none';
						skc.style.display='';
					}
					else
					{
						els[e].style.height='19px';
						sko.style.display='';
						skc.style.display='none';
					}
				}
			}
		}
	}
}

function fxf_fn_hrToggleFilter(cn)
{
	var id=$('hr'+cn+'ftx');
	if(id.style.display == 'none')
	{
		var csn=tSet.hrcsort.substr(0,2);
		if(csn == cn)
			id.style.borderColor='#96d9f9';
		else
			id.style.borderColor='#cbcbcb';
		id.style.display='';
	}
	else
		id.style.display='none';
	fxf_fn_openHRPool();
}

function fxf_fn_hrMarkFilter(html,rid,nmflt,skflt)
{
	html=html.replace(new RegExp('<span name="hrfm"><span><b class="red bold">', 'g'), '').replace(new RegExp('</b></span></span>', 'g'), '');
	for(var t=0; t<2; t++)
	{
		var sts=0;
//<span id="rptd(rid)_nm" ... "> name </span>
		if(!t && nmflt.length)
		{
			var sts=html.indexOf(' id="rptd'+rid+'_nm"');
			var flt=nmflt;
//if(rid == 3103) alert(t+': html='+html+'\n\nflt='+flt);
		}
//<div id="rptd($rid)_sk" ... > skills </div>
		else if((t == 1) && skflt.length)
		{
			var sts=html.indexOf(' id="rptd'+rid+'_sk"');
			flt=skflt;
//if(rid == 3103) alert(t+': html='+html+'\n\nflt='+flt);
		}
		if(sts > 0)
		{
			if(!t)
			{
				var ste=sts+html.substr(sts).indexOf('">');
				var set=ste+html.substr(ste).indexOf('</span>');
				var sto=2;
			}
			else
			{
				var ste=sts+html.substr(sts).indexOf('>');
				var set=ste+html.substr(ste).indexOf('</div>');
				var sto=1;
			}
			var shtml=html.substr(0,ste+sto);
			var otxt=html.substr(ste+sto,set-ste-sto);
			var ntxt='';
			var i=false;
			for(var p=0; p<otxt.length; p++)
			{
				var c=otxt.substr(p,1);
				if(c == '<')
				{
					ntxt += c;
					i=true;
				}
				else if(c == '>')
				{
					ntxt += c;
					i=false;
				}
				else if(i)
					ntxt += c;
				else if((c == '&') && (otxt.substr(p,6) == '&nbsp;'))
				{
					ntxt += '&nbsp;';
					p += 5;
				}
				else
				{
					var n=otxt.substr(p,flt.length).toLowerCase();
					if(n == flt)
					{
						ntxt += '<span name="hrfm"><span><b class="red bold">'+otxt.substr(p,flt.length)+'</b></span></span>';
						p += flt.length-1;
					}
					else
						ntxt += c;
				}
			}
			var ehtml=html.substr(set);
//if(rid == 3103) alert(t+': fxf_fn_hrMarkFilter(rid='+rid+', flt='+flt+')\n\notxt='+otxt+'\n\nntxt='+ntxt);
			html=shtml+ntxt+ehtml;
		}
	}

	return html;
}

function fxf_fn_simClick(element, with_first)
{
	var btn=null;

	var sfrm='';
	if(element && element.id && element.attributes && element.attributes['fxform'])
	{
//alert('SRC: '+element.id);
		sfrm=element.attributes['fxform'].value;
	}
//alert('sfrm='+sfrm);

	var bels=$$('[type="submit"]');
//alert('bels='+bels);
	if(bels.length && ((element && sfrm) || !sfrm.length))
	{
		var fel=null;
		var sel=null;
		var bel=null;
		for(var b=0; b<bels.length; b++)
		{
			var bptr=0;
			if(bels[b].attributes && bels[b].attributes['ptr'])
				bptr=parseInt(bels[b].attributes['ptr'].value);
//alert('B: '+bels[b].id+' - C: '+bels[b].className+' - A: '+bptr);
			if(bels[b].className && (bels[b].className.substr(0,5) == 'fxfbs') && (bptr == oFXP.ptr))
			{
				var dfrm=bels[b].attributes['fxform'].value;
//alert('DST: '+bels[b].id+' - dfrm='+dfrm);
				if((with_first && (dfrm == sfrm)) || !with_first)
				{
					// First button
					if(!fel)
						fel=bels[b];
					// Baseline button
					if(!bel && bels[b].attributes && bels[b].attributes['baseline'])
						bel=bels[b];
					// Standard button
					if(!sel && bels[b].attributes && bels[b].attributes['ref'] && (bels[b].attributes['ref'].value == 's'))
					{
						sel=bels[b];
						break;
					}
				}
			}
		}

		if(sel)
		{
//alert('Click on STANDARD button: id='+sel.id+', value='+sel.value+', class='+sel.className+', fxform='+sel.attributes['fxform'].value);
			sel=fxf_fn_getField(sel,false);
			btn=sel;
		}
		else if(bel)
		{
//alert('Click on BASELINE button: id='+bel.id+', value='+bel.value+', class='+bel.className+', fxform='+bel.attributes['fxform'].value);
			bel=fxf_fn_getField(bel,false);
			btn=bel;
		}
		else if(fel && with_first)
		{
//alert('Click on FIRST button: id='+fel.id+', value='+fel.value+', class='+fel.className+', fxform='+fel.attributes['fxform'].value);
			fel=fxf_fn_getField(fel,false);
			btn=fel;
		}
	}

	return btn;
}

function fxf_fn_message(tid, val)
{
	var msg='';
	switch(tid)
	{
		case 'no-rights':
			if(oFXP.lang == 1)
				msg='Keine Berechtigung für diese Aktion';
			else
				msg='Insufficient rights for this action';
		break;

		case 'not-for-mp':
			if(oFXP.lang == 1)
				msg='Diese Aktion ist bei einem Hauptprojekt nicht möglich';
			else
				msg='This action is not allowed for a main project';
		break;

		case 'project-not-changeeable':
			if(oFXP.lang == 1)
				msg='Dieses Projekt kann nicht gelöscht, geändert oder umbenannt werden, da';
			else
				msg='This project cannot be deleted, changed or renamed, because';
		break;

		case 'task-not-changeable':
			if(oFXP.lang == 1)
				msg='Diese Aufgabe kann nicht gelöscht, geändert oder umbenannt werden, da';
			else
				msg='This task cannot be deleted, changed or renamed, because';
		break;

		case 'project-not-deleteable':
			if(oFXP.lang == 1)
				msg='Dieses Projekt kann nicht gelöscht werden, da';
			else
				msg='This project cannot be deleted, because';
		break;

		case 'task-not-deleteable':
			if(oFXP.lang == 1)
				msg='Diese Aufgabe kann nicht gelöscht werden, da';
			else
				msg='This task cannot be deleted, because';
		break;

		case 'project-not-switchable':
			if(oFXP.lang == 1)
				msg='Dieses Projekt kann nicht zu einer Aufgabe umgeändert werden, da';
			else
				msg='This project cannot be switched to a task, because';
		break;

		case 'task-not-switchable':
			if(oFXP.lang == 1)
				msg='Diese Aufgabe kann nicht zu einem Projekt umgeändert werden, da';
			else
				msg='This task cannot be switched to a project, because';
		break;

		case 'times-recorded':
			if(oFXP.lang == 1)
				msg='<li>bereits Zeiten darauf erfasst wurden</li>';
			else
				msg='<li>times have been already recorded to it</li>';
		break;

		case 'travel-expenses':
			if(oFXP.lang == 1)
				msg='<li>bereits Reisekosten darauf gebucht wurden</li>';
			else
				msg='<li>travel expenses have been already booked to it</li>';
		break;

		case 'material-assigned':
			if(oFXP.lang == 1)
				msg='<li>bereits Material dazugeordnet wurde</li>';
			else
				msg='<li>material has been already assigned to it</li>';
		break;

		case 'int-budget-used':
			if(oFXP.lang == 1)
				msg='<li>bereits internes Budget dafür verbraucht wurde</li>';
			else
				msg='<li>internal budget has been already used on it</li>';
		break;

		case 'out-invoices-created':
			if(oFXP.lang == 1)
				msg='<li>bereits Ausgangsrechnungen dafür erstellt wurden</li>';
			else
				msg='<li>outgoing invoices have been already created for it</li>';
		break;

		case 'inc-invoices-assigned':
			if(oFXP.lang == 1)
				msg='<li>bereits Eingangsrechnung dafür zugeordnet wurden</li>';
			else
				msg='<li>incoming invoices have been already assigned to it</li>';
		break;

		case 'docs-linked':
			if(oFXP.lang == 1)
				msg='<li>aktive Dokumente damit verbunden sind</li>';
			else
				msg='<li>active documents are linked to it</li>';
		break;

		case 'depend-exist':
			if(oFXP.lang == 1)
				msg='<li>bereits zu diesem Projekt abhängige Vorgänge existieren</li>';
			else
				msg='<li>processes depending on this project already exist</li>';
		break;

		case 'bud-frame-exeeded':
			if(oFXP.lang == 1)
				msg='Das geplante Budget dieses Vorgangs würde den Budgetrahmen eines vorherigen Projektes sprengen';
			else
				msg='The planned budget for this process would exceed the budget frame of a previous project';
		break;
	}

	return msg;
}

function fxf_fn_clickFilter(sm)
{
	if(sm)
	{
		oID.fxfltsel.style.width=tblwidth+'px';
		oID.fxsrcinput.style.width='0';
	}
	else
	{
		oID.fxfltsel.style.width=tbswidth+'px';
		oID.fxsrcinput.style.width=tbswidth+'px';
	}
}

function fxf_fn_filterChanged()
{
	fxf_fn_changeSetting(0,'setfilter|afilter',gSet.filter+'|'+oID.fxfltsel.attributes['svalue'].value);

	// Warn user by transaction change if values have been edited and transaction is not in display mode
	if((oFXP.changed > 0) && (oFXP.action > 1) && (oFXP.tr > 0))
	{
		fxf_fn_waitScreen('');
		fxf_fn_question(fxf_fn_getMessage(7,true), fxf_fn_getMessage(7,false), [fxf_fn_getText(10), fxf_fn_getText(11)], ['fxf_fn_resetChanged();fxf_fn_waitScreen(\'reloading\');fxf_fn_loadTR('+oFXP.tr+',\'Reload\');', ''], 100);
		return;
	}
	else
	{
		fxf_fn_waitScreen('reloading');
		fxf_fn_loadTR.delay(oFXP.ldelay, oFXP.tr,'Reload');
	}
}

function fxf_fn_changeWFSetting(sval,change)
{
	// Get new settings value
	var ov=gSet[sval];
	var nv=ov;
	if(change)
	{
		if(nv)
			nv=0;
		else
			nv=1;
	}
	gSet[sval]=nv;
//alert('fxf_fn_changeWFSetting(sval='+sval+')\n\nold value: '+ov+'\nnew value: '+nv);

	var cm='w';
	var ca='b';
	var dsp='';
	if(gSet.wfcolor)
	{
		cm='c';
		ca='c';
		dsp='none';
	}

	// Setting: Action row icons
	if(sval == 'wfairow')
	{
		var wfb0=$('wfbam0');
		var wfb1=$('wfbam1');
		if(nv)
		{
			wfb0.style.display='none';
			wfb1.style.display='';
		}
		else
		{
			wfb0.style.display='';
			wfb1.style.display='none';
		}

		if(change && gSet.wfsissi.length)
		{
			var sissi=gSet.wfsissi.split('_');
			fxf_fn_drawWFContent(parseInt(sissi[0]),parseInt(sissi[1]));
		}
	}

	// Setting: Color icons
	if(sval == 'wfcolor')
	{
		var wfb0=$('wfbcp0');
		var wfb1=$('wfbcp1');
		if(nv)
		{
			wfb0.style.display='none';
			wfb1.style.display='';
		}
		else
		{
			wfb0.style.display='';
			wfb1.style.display='none';
		}

		if(change)
		{
			// ...Change also back...
			var di=$('wfbbtsi');
			if(di)
				di.src='GFX/ret_'+cm+'_80x80.png';
			// ...and action row icons...
			var di=$('wfbam0i');
			if(di)
				di.src='GFX/am0_'+cm+'_80x80.png';
			var di=$('wfbam1i');
			if(di)
				di.src='GFX/am1_'+cm+'_80x80.png';
			// ...as well as all other level 0 workflow icons...
			mo=$$('[id^="wfmmiit"]');
			if(mo && mo.length)
			{
				for(var i=0; i<mo.length; i++)
				{
					var id=mo[i].id.substr(7);
					var im=mo[i].attributes['img'].value;
					mo[i].src='GFX/'+im+'_'+cm+'_80x80.png';
					$('wfmmiib'+id).style.display=dsp;
				}
			}
			// ...as well as all other level 1 workflow icons...
			mo=$$('[id^="wfmumiit"]');
			if(mo && mo.length)
			{
				for(var i=0; i<mo.length; i++)
				{
					var id=mo[i].id.substr(8);
					var im=mo[i].attributes['img'].value;
					mo[i].src='GFX/'+im+'_'+cm+'_80x80.png';
					$('wfmumiib'+id).style.display=dsp;
				}
			}
			// ...as well as all content workflow action icons
			mo=$$('[id^="fxm_wfmcmii"]');
			if(mo && mo.length)
			{
				for(var i=0; i<mo.length; i++)
				{
					var am=mo[i].id.substr(mo[i].id.length-1,1);
					mo[i].src='GFX/ac'+am+'_'+ca+'_80x80.png';
				}
			}
		}
	}

	// Save changed global workflow setting
	if(change)
		fxf_fn_changeSetting(0,'set'+sval,gSet[sval]);
}

function fxf_fn_getWFMenu(mod,wid)
{
	var ret='';
	var url=fxf_fn_gProgram('sld_workflow', 'mod='+mod+'&wid='+wid+'&lng='+oFXP.lang+'&col='+gSet.wfcolor+'&aim='+gSet.wfairow+fxf_fn_gParam());
//alert('fxf_fn_getWFMenu(mod='+mod+',wid='+wid+')\nurl='+url);
	new Ajax.Request(url,
	{
		method:'get', asynchronous:false,
		onSuccess: function(transport)
		{
			ret=transport.responseText;
//alert(ret);
		}
	});

	return ret;
}

function fxf_fn_drawWFMenu(radius,animate)
{
//alert('fxf_fn_drawWFMenu(radius='+radius+', animate='+animate+')');
	var mh='';
	var mo=[];

	if(animate)
	{
		var id_hi=$('wfmhi');
		if(id_hi)
			mh=id_hi.innerHTML;
		mo=$$('[id^="wfmsi"]');
	}
	else
	{
		var s=fxf_fn_getWFMenu(0,'');
		if(s.length)
		{
			var hm=s.split('^');
			mh=hm[0];
			if(hm.length > 1)
			{
				var sa=hm[1].split('°');
				for(var i=0; i<sa.length; i++)
				{
					var ti=sa[i].split('|');
					mo[i]={'id':ti[0], 'txt':ti[1], 'img':ti[2], 'gc1':ti[3], 'gc2':ti[4], 'brd':ti[5]};
				}
			}
		}
	}

	gSet.wfsissi='';
	gSet.wfradius=radius;
	var m=parseInt(radius*2.75)+10;
	var r=Math.min(parseInt(m/4), parseInt(m*1.7/mo.length));

	// Set container dimension
	oID.iworkflow.style.width=m+'px';

	var ld=14;
	var td=-14;

	if(animate)
	{
		// Work
		var id_wc=$('wfmwc');
		if(id_wc.style.display != 'none')
			new Effect.Fade(id_wc, {from:1.0, to:0.0, duration: 0.5});
		else
			id_wc.style.display='none';

		// Submenu
		var id_ui=$('wfmui');
		new Effect.Fade(id_ui, {from:1.0, to:0.0, duration: 0.5});

		// Headline
		if(mh.length)
		{
			var id_hi=$('wfmhi');
			if(id_hi)
				new Effect.Morph(id_hi, {style:'opacity:1;', duration: 0.5});
		}

		// Circle
		var id_ci=$('wfmci');
		new Effect.Morph(id_ci, {style:'opacity:1;', duration: 0.5});

		// Items
		for(var i=0; i<mo.length; i++)
		{
			var a=0.0;
			if(i>0)
				a=i*(360.0/mo.length);
			var s=Math.sin((a-90.0) * Math.PI / 180.0)*radius;
			var c=Math.cos((a-90.0) * Math.PI / 180.0)*radius;

			var id_si=$('wfmsi'+i);
			var id_mi=$('wfmmi'+i);
			var id_ti=$('wfmti'+i);

			id_si.attributes['tooltip'].value='';
			id_mi.className='wfmcircle1_sel';
			id_ti.style.display='';
			id_ti.style.top='';
			id_ti.style.left='';

			new Effect.Morph(id_si, {style:'left:'+parseInt(m/2+c-r/2-ld)+'px; top:'+parseInt(m/2+s-r/2-td)+'px; width:'+r+'px; height:'+r+'px; opacity:1;', duration: 0.5});
			new Effect.Morph(id_ti, {style:'opacity:1;', duration: 0.5});
		}
	}
	else
	{
		var cm='w';
		var dsp='';
		if(gSet.wfcolor)
		{
			cm='c';
			dsp='none';
		}

		var h='';

		// Setting: Action row icons
		fxf_fn_changeWFSetting('wfairow',false);

		// Setting: Color icons
		fxf_fn_changeWFSetting('wfcolor',false);

		// Headline
		if(mh.length)
			h += '<div id="wfmhi" style="position:absolute;left:8px;top:4px;color:#ffffff;font-size:13pt;font-weight:bold;text-shadow:1px 1px #000000;">'+mh+'</div>';

		// Circle
		h += '<div id="wfmci" style="position:absolute;left:'+parseInt(m/2-radius-ld)+'px;top:'+parseInt(m/2-radius-td)+'px;width:'+parseInt(radius*2)+'px;height:'+parseInt(radius*2)+'px;border:1px solid #000000;border-radius:50%;opacity:0.25;"></div>';

		// Submenu
		h += '<div id="wfmui" style="position:absolute;left:110px;top:110px;right:10px;bottom:10px;background:rgba(0,0,0, 0.25);border:2px solid #ffffff;border-right-color:#333344;border-bottom-color:#333344;border-radius:15px;box-shadow:rgba(0,0,0,0.66) 4px 4px 8px inset;opacity:0;display:none;"></div>';

		// Work
		h += '<div id="wfmwc" style="position:absolute;left:113px;top:113px;right:13px;bottom:13px;background:linear-gradient(to bottom right,#f0f0f0,#d0d0d0);border-radius:13px;display:none;">';
		h += '	<div id="wfmwcc" style="position:absolute;left:6px;top:4px;right:6px;bottom:4px;padding-top:12px;overflow-x:auto;overflow-y:auto;"></div>';
		h += '</div>';

		// Items
		for(var i=0; i<mo.length; i++)
		{
			var a=0.0;
			if(i>0)
				a=i*(360.0/mo.length);
			var s=Math.sin((a-90.0) * Math.PI / 180.0)*radius;
			var c=Math.cos((a-90.0) * Math.PI / 180.0)*radius;

			var dsp='';
			if(gSet.wfcolor)
				dsp='none';

			h += '<div id="wfmsi'+i+'" class="wfmcircle0" wid="'+mo[i].id+'" tooltip="" style="position:absolute;left:'+parseInt(m/2+c-r/2-ld)+'px;top:'+parseInt(m/2+s-r/2-td)+'px;width:'+r+'px;height:'+r+'px;opacity:1.0;z-index:9995;" onclick="fxf_fn_drawWFSubmenu1('+radius+','+i+','+(i+1)+');">';
			h += '	<div id="wfmmi'+i+'" class="wfmcircle1_sel" style="background:linear-gradient(to bottom right,'+mo[i].gc1+','+mo[i].gc2+');'+mo[i].brd+'"><div class="wfmcircle2" style="background:linear-gradient(to bottom right,#f0f0f0,'+mo[i].gc1+');border:1px solid '+mo[i].gc2+';"><img id="wfmmiit'+i+'" class="wfmimage" img="'+mo[i].img+'" src="GFX/'+mo[i].img+'_'+cm+'_80x80.png" style="left:19%;top:19%;"><img id="wfmmiib'+i+'" class="wfmimage" src="GFX/'+mo[i].img+'_b_80x80.png" style="display:'+dsp+';"></div></div>';
			h += '	<div id="wfmti'+i+'" class="wfmtextpos"><font id="wfmtfi'+i+'" class="wfmtext">'+mo[i].txt+'</font></div>';
			h += '</div>';
//alert(i+': a='+a+', r='+r+' -- s='+s+', c='+c);
		}

		oID.iworkflowi.innerHTML=h;
	}

	// Setting: Back
	$('wfbbts').style.display='none';

	// Workflow background
	oID.iworkflowi.style.background='linear-gradient(to bottom right,rgba(0,0,0, 0.25),rgba(0,0,0, 0.5))';
}

function fxf_fn_drawWFSubmenu1(radius,si,i)
{
//alert('fxf_fn_drawWFSubmenu1(radius='+radius+', si='+si+', i='+i+')');
	var mo=$$('[id^="wfmsi"]');

	if(i >= mo.length)
		i=0;

	// Circle
	var id_ci=$('wfmci');
	if(id_ci.getOpacity() == 0.0)
		i=si;

	// Items
	if(i == si)
	{
		gSet.wfsissi='';

		// Set container dimension
		var m=parseInt(radius*2.75)+10;
		oID.iworkflow.style.width=m+'px';

		// Actual Item
		var id_si=$('wfmsi'+i);
		var id_mi=$('wfmmi'+i);
		var id_ti=$('wfmti'+i);

		id_si.setOpacity(1.0);
		id_si.attributes['tooltip'].value='';
		id_mi.className='wfmcircle1_act';
		id_ti.style.display='';

		new Effect.Morph(id_si, {style:'top:10px; left:10px; width:80px; height:80px;', duration: 0.5});
		new Effect.Morph(id_ti, {style:'top:91px; left:100px; opacity:1.0;', duration: 0.5});

		// Work
		var id_wc=$('wfmwc');
		if(id_wc.style.display != 'none')
			new Effect.Fade(id_wc, {from:1.0, to:0.0, duration: 0.5});
		else
			id_wc.style.display='none';

		// Headline
		var id_hi=$('wfmhi');
		if(id_hi)
			new Effect.Morph(id_hi, {style:'opacity:0;', duration: 0.5});

		// Circle
		new Effect.Morph(id_ci, {style:'opacity:0;', duration: 0.5});

		// Other Items
		var lh=m-122;
		var ta=Math.floor(lh/10);
		var td=Math.floor(ta*0.75);
		var tl=Math.floor(50-(td/2));
//alert('m='+m+' -- lh='+lh+' -- ta='+ta+', td='+td+', tl='+tl);
		var t=110;
		for(i=0; i<mo.length; i++)
		{
			if(i != si)
			{
				var id_si=$('wfmsi'+i);
				var id_mi=$('wfmmi'+i);
				var id_ti=$('wfmti'+i);

				id_mi.className='wfmcircle1_sel';
				id_si.attributes['tooltip'].value='<hide>'+id_ti.innerHTML;

				new Effect.Morph(id_si, {style:'left:'+tl+'px; top:'+t+'px; width:'+td+'px; height:'+td+'px; opacity:0.90;', duration: 0.5});
				new Effect.Morph(id_ti, {style:'opacity:0;', duration: 0.5});

				t += ta;
			}
		}

		var mh='';
		var ms=[];
		var d=0.5;
		var id_si=$('wfmsi'+si);
		var s=fxf_fn_getWFMenu(1,id_si.attributes['wid'].value);
		if(s.length)
		{
			var hm=s.split('^');
			mh=hm[0];
			if(hm.length > 1)
			{
				var sa=hm[1].split('°');
				for(var i=0; i<sa.length; i++)
				{
					var ti=sa[i].split('|');
					ms[i]={'id':ti[0], 'txt':ti[1], 'img':ti[2], 'gc1':ti[3], 'gc2':ti[4], 'brd':ti[5]};
				}
			}
		}

		var id_ui=$('wfmui');
		var dis=true;
		if(id_ui.style.display != '')
			dis=false;

		if(!dis)
		{
			id_ui.setOpacity(0);
			id_ui.style.display='';
		}

		var u=fxf_fn_getBCR(id_ui);
		var m=u.width;
		var radius=parseInt(m/3.2);
		var r=Math.min(parseInt(m/5), parseInt(m*1.3/ms.length));
		var to=parseInt((u.height-m)/2);

		var cm='w';
		var dsp='';
		if(gSet.wfcolor)
		{
			cm='c';
			dsp='none';
		}

		var h='';

		// Headline
		if(mh.length)
			h += '<div id="wfmuhi" style="position:absolute;left:15px;top:15px;font-size:11pt;font-weight:bold;color:#ffffff;text-shadow:1px 1px #000000;">'+mh+'</div>';

		// Circle
		h += '<div id="wfmuci" style="position:absolute;left:'+parseInt(m/2-radius)+'px;top:'+(to+parseInt(m/2-radius))+'px;width:'+parseInt(radius*2)+'px;height:'+parseInt(radius*2)+'px;border:1px solid #000000;border-radius:50%;opacity:0.25;"></div>';

		// Items
		var lgb='';
		for(var i=0; i<ms.length; i++)
		{
			var a=0.0;
			if(i>0)
				a=i*(360.0/ms.length);
			var s=Math.sin((a-90.0) * Math.PI / 180.0)*radius;
			var c=Math.cos((a-90.0) * Math.PI / 180.0)*radius;

			h += '				<div id="wfmusi'+si+'_'+i+'" class="wfmcircle0" wid="'+ms[i].id+'" tooltip="" style="position:absolute;left:-102px;top:-52px;width:80px;height:80px;opacity:0;z-index:9996;" onclick="fxf_fn_drawWFSubmenu2('+radius+','+si+', '+i+', '+(i+1)+');">';
			h += '					<div id="wfmumi'+si+'_'+i+'" class="wfmscircle1_sel" style="background:linear-gradient(to bottom right,'+ms[i].gc1+','+ms[i].gc2+');'+ms[i].brd+'"><div class="wfmscircle2" style="border:1px solid '+ms[i].gc2+';");"><img id="wfmumiit'+si+'_'+i+'" class="wfmimage" img="'+ms[i].img+'" src="GFX/'+ms[i].img+'_'+cm+'_80x80.png" style="left:19%;top:19%;"><img id="wfmumiib'+si+'_'+i+'" class="wfmimage" src="GFX/'+ms[i].img+'_b_80x80.png" style="display:'+dsp+';"></div></div>';
			h += '					<div id="wfmuti'+si+'_'+i+'" class="wfmtextpos"><font id="wfmutfi'+si+'_'+i+'" class="wfmtext">'+ms[i].txt+'</font></div>';
			h += '				</div>';
//alert(i+': a='+a+', r='+r+' -- s='+s+', c='+c);

			if(!lgb.length)
				lgb=ms[i].gc2;
		}

		id_ui.innerHTML=h;

		if(!dis)
			new Effect.Fade(id_ui, {from:0.0, to:1.0, duration: d});

		// Items
		for(var i=0; i<ms.length; i++)
		{
			var a=0.0;
			if(i>0)
				a=i*(360.0/ms.length);
			var s=Math.sin((a-90.0) * Math.PI / 180.0)*radius;
			var c=Math.cos((a-90.0) * Math.PI / 180.0)*radius;

			var iid=$('wfmusi'+si+'_'+i);
			new Effect.Morph(iid, {style:'left:'+parseInt(m/2+c-r/2)+'px; top:'+parseInt(to+m/2+s-r/2)+'px; width:'+r+'px; height:'+r+'px;', duration: d*(1+Math.random()*2)});
			new Effect.Fade(iid, {from:0.0, to:1.0, duration: d*2});
		}

		// Setting: Back
		$('wfbbts').style.display='';

		// Workflow background
		if(lgb.length)
		{
			var lgba=fxf_fn_hexToRgb(lgb,false);
			oID.iworkflowi.style.background='linear-gradient(to bottom right,rgba('+lgba.r+','+lgba.g+','+lgba.b+', 0.25),rgba('+lgba.r+','+lgba.g+','+lgba.b+', 0.5))';
		}
		else
			oID.iworkflowi.style.background='linear-gradient(to bottom right,rgba(0,0,0, 0.25),rgba(0,0,0, 0.5))';
	}
	else
	{
		var id_si=$('wfmsi'+i);
		var id_ti=$('wfmti'+i);

		id_si.setOpacity(0.33);
		id_ti.style.display='none';

		fxf_fn_drawWFSubmenu1.delay(0.02, radius,si,i+1);
	}
}

function fxf_fn_drawWFSubmenu2(radius,si,ssi,i)
{
//alert('fxf_fn_drawWFSubmenu2(radius='+radius+', si='+si+', ssi='+ssi+', i='+i+')');
	var ms=$$('[id^="wfmusi'+si+'_"]');

	if(i >= ms.length)
		i=0;

	// Circle
	var id_uci=$('wfmuci');
	if(id_uci.getOpacity() == 0.0)
		i=ssi;

	// Items
	if(i == ssi)
	{
		// Actual Item
		var id_ssi=$('wfmusi'+si+'_'+i);
		var id_smi=$('wfmumi'+si+'_'+i);
		var id_sti=$('wfmuti'+si+'_'+i);

		var id_tfi=$('wfmtfi'+si);
		var tfid=fxf_fn_getBCR(id_tfi);

		id_ssi.setOpacity(1.0);
		id_ssi.attributes['tooltip'].value='';
		id_smi.className='wfmcircle1_act';
		id_sti.style.display='';

		new Effect.Morph(id_ssi, {style:'top:-100px; left:0; width:70px; height:70px;', duration: 0.5});
		new Effect.Morph(id_sti, {style:'top:89px; left:'+(tfid.width-10)+'px; opacity:1.0;', duration: 0.5});

		// Headline
		var id_uhi=$('wfmuhi');
		if(id_uhi)
			new Effect.Morph(id_uhi, {style:'opacity:0;', duration: 0.5});

		// Circle
		new Effect.Morph(id_uci, {style:'opacity:0;', duration: 0.5});

		// Other Items
		var l=90;
		for(i=0; i<ms.length; i++)
		{
			if(i != ssi)
			{
				var id_usi=$('wfmusi'+si+'_'+i);
				var id_umi=$('wfmumi'+si+'_'+i);
				var id_uti=$('wfmuti'+si+'_'+i);

				id_umi.className='wfmcircle1_sel';
				id_usi.attributes['tooltip'].value='<hide>'+id_uti.innerHTML;

				new Effect.Morph(id_usi, {style:'left:'+l+'px; top:-90px; width:46px; height:46px; opacity:0.90;', duration: 0.5});
				new Effect.Morph(id_uti, {style:'opacity:0;', duration: 0.5});

				l += 60;
			}
		}

		var nw=Math.min(1248, dim.wd.width-64);
		new Effect.Morph(oID.iworkflow, {style:'width:'+nw+'px;', duration: 0.5});

		fxf_fn_drawWFContent.delay(0.6, si,ssi);
	}
	else
	{
		var id_usi=$('wfmusi'+si+'_'+i);
		var id_uti=$('wfmuti'+si+'_'+i);

		id_usi.setOpacity(0.33);
		id_uti.style.display='none';

		fxf_fn_drawWFSubmenu2.delay(0.02, radius,si,ssi,i+1);
	}
}

function fxf_fn_drawWFContent(si,ssi)
{
//alert('fxf_fn_drawWFContent(si='+si+', ssi='+ssi+')');
	var id_wc=$('wfmwc');
	var id_wcc=$('wfmwcc');
	id_wcc.innerHTML='<table width="100%" height="100%"><tr><td width="100%" height="100%" align="center"><font style="font-size:16pt;font-weight:bold;color:#ffffff;text-shadow:1px 1px #000000;">Loading...</font></td></tr></table>';

	gSet.wfsissi=si+'_'+ssi;

	var d=0.0;
	if(id_wc.style.display == 'none')
	{
		d=0.50;

		id_wc.setOpacity(0);
		id_wc.style.display='';

		new Effect.Fade(id_wc, {from:0.0, to:1.0, duration: d});
	}

	fxf_fn_loadWFContent.delay(d, si,ssi);
}

function fxf_fn_loadWFContent(si,ssi)
{
	var id_usi=$('wfmusi'+si+'_'+ssi);
	var h=fxf_fn_getWFMenu(2,id_usi.attributes['wid'].value);
//alert('fxf_fn_drawWFContent('+si+','+ssi+')\n\nvalue='+id_usi.attributes['wid'].value+'\n\nh='+h);

	var id_wcc=$('wfmwcc');
	id_wcc.innerHTML=h;
}

function fxf_fn_formatTimespan(td)
{
	if(!oID.fxtaskf)
		return;

	if(oFXP.tr && oSet && oSet.timedec && oSet.timedec.length)
	{
		if(td.length)
		{
			oSet.timedec=td;
			fxf_fn_saveUserSetting('zeitspanne_dez='+oSet.timedec);
		}

		if(oSet.timedec == 'j')
			oSet.tsformat=oSet.decimalpoint;
		else
			oSet.tsformat=':';

		// Get all elements with the "sec" attribute
		var tsfa=$$('[sec]');
//alert('checkTimespanFormat:\n\ntd='+td+', oSet.timedec='+oSet.timedec+', oSet.tsformat='+oSet.tsformat+'\n\ntshd ('+tsfa.length+') = '+tsfa);

		oID.fxtaskf.innerHTML='0'+oSet.tsformat+'00';
		if(tsfa.length)
		{
			oID.fxtaskf.style.cursor='pointer';
			oID.fxtaskf.setOpacity(1.0);
			if(oFXP.lang == 1)
				var tstt='Zeitspannenformat umschalten zwischen<br />Stundenformat &quot;<b>0:00</b>&quot; oder Dezimalformat &quot;<b>0'+oSet.decimalpoint+'00</b>&quot;';
			else
				var tstt='Switch timespan format between<br />hour format &quot;<b>0:00</b>&quot; or decimal format &quot;<b>0'+oSet.decimalpoint+'00</b>&quot;';
		}
		else
		{
			oID.fxtaskf.style.cursor='default';
			oID.fxtaskf.setOpacity(0.50);
			var tstt='';
		}
		oID.fxtaskf.attributes['tooltip'].value=tstt;
		oID.fxtaskf.style.display='';

		// Adjust timespan?
		if(td.length && tsfa.length)
		{
			var frs='';
			var frm='';
			for(var t=0; t<tsfa.length; t++)
			{
				if(!tsfa[t].type || (tsfa[t].type != 'hidden'))
				{
					if(tsfa[t].attributes && tsfa[t].attributes['value'])
					{
						var v=true;
						var o=tsfa[t].value;
					}
					else
					{
						var v=false;
						var o=tsfa[t].innerHTML;
					}
					var n=o;
					var s=parseInt(tsfa[t].attributes['sec'].value);
					if(o.length)
					{
						var cp=o.indexOf(':');
						if((cp < 0) && (oSet.tsformat == ':'))			// ...to hour format
							var n=fxf_fn_sec2time(s,false);
						else if((cp >= 0) && (oSet.tsformat != ':'))	// ...to decimal format
							var n=fxf_fn_float2string(Math.round(s/36) / 100);
						if(v)
							tsfa[t].value=n;
						else
							tsfa[t].innerHTML=n;
					}
					if(v && (o != n))
					{
						if(!frm.length && tsfa[t].attributes && tsfa[t].attributes['fxform'])
							frm=tsfa[t].attributes['fxform'].value;
						if(frs.length)
							frs += '&';
						frs += tsfa[t].id+'=tx'+n;
					}
				}
			}

			// Save changed fields?
			if(frs.length && frm.length)
			{
//alert('Save changed fields: frm='+frm+'\n\n'+frs);
				fxf_fn_saveFields.delay(oFXP.ldelay, frs,'frm='+frm);
			}
		}
	}
	else
		oID.fxtaskf.style.display='none';
}

function fxf_fn_timespan2sec(ts, id, ti)
{
	var si='';
	var sec='';
	if(ts.length)
	{
		var no='';
		var nm='';
		var df=0;
		var hf=0;
		var it=false;
		for(var z=0; z<ts.length; z++)
		{
			var c=ts.substr(z,1);
			if(it)
			{
				if(c == '>')
					it=false;
			}
			else
			{
				if(c == '<')
					it=true;
				else if((c == '0') || (parseInt(c) > 0))
				{
					if(!no.length && (z > 0) && (ts.substr(z-1,1) == '-'))
						si='-';
					if(hf)
						nm += c+'';
					else
						no += c+'';
				}
				else if((c == ',') || (c == '.'))
				{
					if(df)
						no=no.replace(/\./g, '');
					no += '.';
					df=1;
				}
				else if((c == ':') || (c == ' '))
				{
					hf=1;
				}
			}
		}
		if(no.length)
		{
			if(hf)	// hour format to sec
			{
				df=0;
				no=no.replace(/\./g, '');

				var h=parseInt(no);
				var m=parseInt(nm);

				sec=h*3600 + m*60;
			}
			else if(df)	// decimal format to sec
			{
				var no=parseFloat(no);
				var h=parseInt(no);
				var m=parseInt((no-h) * 60);

				sec=h*3600 + m*60;
			}
			else if((id != undefined) && (id.substr(0,5).toLowerCase() == 'pause'))	// minutes to sec
				sec=parseInt(no)*60;
			else	// hours to sec
				sec=parseInt(no)*3600;
		}
//alert('ts='+ts+'\n\nnumber(no)='+no+', minutes(nm)='+nm+', sign(si)='+si+', decimals(df)='+df+', hours(hf)='+hf+'\n\nsec='+sec);
	}

	if((typeof ti == 'undefined') || (ti == '') || !ti)
		return si+sec;

	if(sec.length == 0)
		return 0;

	return parseInt(si+sec);
}

function fxf_fn_sec2timespan(sec)
{
	if(oSet.timedec == 'j')	// ...to decimal format
		var t=fxf_fn_float2string(Math.round(sec/36) / 100);
	else					// ...to hour format
		var t=fxf_fn_sec2time(sec,false);
//alert('fxf_fn_sec2timespan(sec='+sec+')\noSet.timedec='+oSet.timedec+'\n-> '+t);

	return t;
}

function fxf_fn_deletePic(ask, uparam)
{
	var fb=$('bild');
	var frm='';
	if(fb && fb.attributes && (typeof fb.attributes['fxform'] != 'undefined'))
		frm=fb.attributes['fxform'].value;

	var param='';
	if(uparam.length)
		param=uparam;
	else
	{
		var fp=$('uparam');
		if(fp)
			param=fp.value;
	}

	if(ask)
	{
		var mode=param.substr(0,3);
		if(oFXP.lang == 1)
		{
			var h='Bild löschen?';
			if(mode == 'PER')
				var q='Wollen Sie das Bild für diese Firma/Person wirklich löschen?';
			else if(mode == 'MAT')
				var q='Wollen Sie das Bild für dieses Material wirklich löschen?';
			else
				var q='Wollen Sie dieses Bild wirklich löschen?';
		}
		else
		{
			var h='Delete Picture?';
			if(mode == 'PER')
				var q='Do you really want to delete the picture for this company/person?';
			else if(mode == 'MAT')
				var q='Do you really want to delete the picture for this material?';
			else
				var q='Do you really want to delete this picture?';
		}
		fxf_fn_question(h, q, [fxf_fn_getText(10), fxf_fn_getText(11)], ['fxf_fn_deletePic(false, \''+uparam+'\');', ''], 100);
		return;
	}

	fxf_fn_waitScreen('#');

	var url=fxf_fn_gProgram('delpic', 'keep_post=1&tr='+oFXP.tr+'&lang='+oFXP.lang+fxf_fn_gParam());
	var pst='frm='+frm+'&param='+fxf_fn_escapeText(param,false);
//alert('url='+url+'\n\npst='+pst);

	new Ajax.Request(url,
	{
		method:'post', postBody:pst, asynchronous:true,
		onSuccess: function(transport)
		{
			var rt=transport.responseText;
			var rs=rt.split('|');
//alert('rt:\n'+rt+'\n\nrs:\n'+rs);
//alert('ifc: (uld='+uld+', ifs='+ifs+')\n\n'+ifc);
			if(rs[0] == '001')	// OK (green)
			{
				if(uparam.length)
				{
					var ps=param.split('|');
					var id=ps[3];
					var mi=$('mat_'+id+'_icon');
					if(mi)
					{
						if(oFXP.lang == 1)
							mi.attributes['tooltip'].value='Kein Bild vorhanden!';
						else
							mi.attributes['tooltip'].value='No picture available!';
						mi.style.boxShadow='';
						mi.innerHTML='<img src="'+rs[1]+'" style="opacity:0.15;">';
					}
				}
				else
				{
					var db=$('del_bild');
					if(db)
						db.style.display='none';
					var dd=$('div_bild');
					if(dd && rs[1].length)
					{
						fb.src=rs[1];
						var dw=parseInt(dd.style.width);
						var dh=parseInt(dd.style.height);
						var nw=100;
						var nh=100;
						if(dw && (100 > dw))
						{
							nw=dw;
							nh=Math.floor(nw*100 / 100);
						}
						if(dh && (nh > dh))
						{
							nw=Math.floor(nw*dh / nh);
							nh=dh;
						}
						fb.style.width='100px';
						fb.style.height='100px';
						if(nh < dh)
							fb.style.top=Math.floor((dh-nh)/2)+'px';
						else
							fb.style.top='';
						dd.setOpacity(0.10);
					}
					else
						fb.style.display='none';
				}
			}
			fxf_fn_waitScreen('');
			fxf_fn_showMessage(rs[0], rs[2]);
		}
	});
}

function fxf_fn_pdfToggleFit()
{
	var cb=$('pdfs_fit');
	var af=fxf_fn_getElement('pdfs_fac',0,'text');
	var of=$('pdfs_ofac');

	if(cb.checked)
	{
		of.value=fxf_fn_string2float(af.value);
		af.className='fxftxg';
		af.value=fxf_fn_float2string(100.0);
		af.style.color='#888888';
		af.readOnly=true;
		af.disabled=true;
	}
	else
	{
		if(af.disabled)
			af.disabled=false;
		if(af.readOnly)
			af.readOnly=false;
		af.className='fxftx';
		af.style.color='';
		af.value=fxf_fn_float2string(of.value);
	}

	for(var c=0; c<3; c++)
	{
		var td=$('pdfdm_s'+c);
		if(cb.checked)
			td.className='lightergrey';
		else
			td.className='';
	}
}

function fxf_fn_showMessage(st, msg)
{
	if(msg == undefined)
		msg='';
	if(msg.length)
		fxf_fn_taskMsg(msg.replace(/\\\\/g, '\\'));

	if(st.length)
	{
		stype=fxf_fn_setMessageStatus(st);
		if(oFXP.ptr)
			oID.ptmessagem.innerHTML=msg;
		else
			oID.imessagem.innerHTML=msg;
		if(oID.fxframe.style.display == 'none')
		{
			fxf_fn_growMessage.delay(2.0, 0.5);
			fxf_fn_shrinkMessage.delay(4.0, 0.3,false);
		}
		else
		{
			fxf_fn_growMessage(0.5);
			fxf_fn_shrinkMessage.delay(3.0, 0.3,false);
		}
	}
	else
	{
		fxf_fn_taskMsg('');
		if(oFXP.ptr)
		{
			oID.ptmessage.style.display='none';
			if(oFXP.tr != 110)
				oID.ptbuttons.style.display='none';
		}
		else
		{
			oID.imessage.style.display='none';
			if((oFXP.tr != 107) && (oFXP.tr != 110))
				oID.fxbuttons.style.display='none';
		}
	}
}

function fxf_fn_convBytes(val, from, round)
{
	// Convert (T/G/M/K/B) to B
	if((typeof from === 'boolean') && !from)
	{
		var n='';
		var u='';
		var f=0;
		var b=0;
		for(var l=0; l<val.length; l++)
		{
			var c=val.substr(l,1).toUpperCase();
			var i=parseInt(c)+'';
			if(c == '.')
				f=1;
			else if(c === i)
				n += i;
			else if((c === 'B') || (c === 'K') || (c === 'M') || (c === 'G') || (c == 'T'))
			{
				u=c;
				break;
			}
		}
		if(f)
			n=parseFloat(f);
		else
			n=parseInt(n);
		if(u == 'T')
			b=parseInt(n*1099511627776);
		else if(u == 'G')
			b=parseInt(n*1073741824);
		else if(u == 'M')
			b=parseInt($n*1048576);
		else if(u == 'K')
			b=parseInt(n*1024);
		else
			b=parseInt(n);
	}
	// Convert B to (T/G/M/K/B)
	else if(((typeof from === 'boolean') && from) || from.length)
	{
		val=parseInt(val);
		var u='';
		var b=0.0;
		if(typeof from === 'string')
		{
			u=from.trim().substr(0,1).toUpperCase();
			if((u != 'T') && (u != 'G') && (u != 'M') && (u != 'K') && (u != 'B'))
				u='';
		}
		if(u === 'T')
			b=val/1099511627776;
		else if(u === 'G')
			b=val/1073741824;
		else if(u === 'M')
			b=val/1048576;
		else if(u === 'K')
			b=val/1024;
		else if(u === 'B')
			b=val;
		else if(val >= 1099511627776)
		{
			b=val/1099511627776;
			u='T';
		}
		else if(val >= 1073741824)
		{
			b=val/1073741824;
			u='G';
		}
		else if(val >= 1048576)
		{
			b=val/1048576;
			u='M';
		}
		else if(val >= 1024)
		{
			b=val/1024;
			u='K';
		}
		else
		{
			b=val;
			u='B';
		}

		if(round)
			b=fxf_fn_float2string(Math.ceil(b), 0);
		else
			b=fxf_fn_float2string(Math.ceil(b*100)/100, 2);

		b += ' '+u;
		if(u != 'B')
			b += 'B';
	}

	return b;
}

function fxf_fn_drawColumn(div, uid, fname,ftype,dtype,width,align,def, value, col)
{
	// Text
	if(dtype == 6)
	{
		var cs='';
		if((ftype == 6) || (ftype == 11) || (ftype == 16) || (ftype == 17) || (ftype == 50) || (ftype == 51) || (ftype == 52))
		{
			if(value < 0.0)
			{
				value=0.0;
				cs='color:#e1001a;';
			}
			else if(value == 0.0)
				cs='color:#3b3b3b;';
			cs += 'text-align:right;';
		}
		// ...Decimal
		if(ftype == 6)
			div.innerHTML='<div id="d_'+fname+'['+uid+']" style="position:relative;top:0;left:0;height:'+dh+'px;margin:0 1px;z-index:24;"><input id="'+fname+'['+uid+']" name="'+fname+'['+uid+']" class="fmftx" type="text" value="'+fxf_fn_float2string(value)+'" ref="fx_ta_425" style="margin-top:1px;width:'+(width-9)+'px;height:'+(dh-9)+'px;'+cs+'"></div>';
		// ...Date
		else if(ftype == 7)
			div.innerHTML='<div id="d_'+fname+'['+uid+']" style="position:relative;top:0;left:0;height:'+dh+'px;margin:0 1px;z-index:24;"><input id="'+fname+'['+uid+']" name="'+fname+'['+uid+']" class="fmftx" type="text" value="'+fxf_fn_dateFormat(value)+'" maxlength="10" style="margin-top:1px;width:'+(width-27)+'px;height:'+(dh-9)+'px;text-align:center;"><img id="img_'+fname+'['+uid+'" class="date_pop fxfil" src="GFX/im_calendar.png" style="margin:0 1px;"></div>';
		// ...Budget
		else if((ftype == 11) || (ftype == 16) || (ftype == 17))
		{
			var wr=24;
			if(oSet.currency_length > 1)
				wr += (oSet.currency_length-1)*7;
//alert('pc='+pc+', cc='+cc+', fname='+fname+', dtype='+dtype+', ftype='+ftype+', value='+value+' - wr='+wr);
			div.innerHTML='<div id="d_'+fname+'['+uid+']" style="position:relative;top:0;right:0;height:'+dh+'px;margin:0 1px;text-align:right;z-index:24;"><input id="'+fname+'['+uid+']" name="'+fname+'['+uid+']" class="fmftx" type="text" value="'+fxf_fn_float2string(value)+'" ref="fx_ta_425" style="margin-top:1px;width:'+(width-wr)+'px;height:'+(dh-9)+'px;text-align:right;'+cs+'">&nbsp;<font class="fmfro">'+oSet.currency+'</font></div>';
		}
		// ...Duration
		else if(ftype == 50)
		{
			var wd=38;
			if(gSet.dunit == 5)
				wd=32;
			else if(gSet.dunit == 4)
				wd=34;
			else if(gSet.dunit > 2)
				wd=28;
			div.innerHTML='<div id="d_'+fname+'['+uid+']" style="position:relative;top:0;right:0;height:'+dh+'px;margin:0 1px;text-align:right;z-index:24;"><input id="'+fname+'['+uid+']" name="'+fname+'['+uid+']" class="fmftx" type="text" value="'+fxf_fn_sec2unit(value,-1)+'" ref="fx_ta_425" style="margin-top:1px;width:'+(width-wd)+'px;height:'+(dh-9)+'px;text-align:right;'+cs+'">&nbsp;<font class="fmfro">'+oSet.unitlit[gSet.dunit]+'</font></div>';
		}
		// ...Costs per Hour
		else if(ftype == 51)
		{
			var wr=50;
			if(oSet.currency_length > 1)
				wr += (oSet.currency_length-1)*8;
			div.innerHTML='<div id="d_'+fname+'['+uid+']" style="position:relative;top:0;right:0;height:'+dh+'px;margin:0 1px;text-align:right;z-index:24;"><input id="'+fname+'['+uid+']" name="'+fname+'['+uid+']" class="fmftx" type="text" value="'+fxf_fn_float2string(value)+'" ref="fx_ta_425" style="margin-top:1px;width:'+(width-wr)+'px;height:'+(dh-9)+'px;text-align:right;'+cs+'">&nbsp;<font class="fmfro">'+oSet.currency+'/'+oSet.unitlit[2]+'</font></div>';
		}
		// ...Percentage
		else if(ftype == 52)
			div.innerHTML='<div id="d_'+fname+'['+uid+']" style="position:relative;top:0;right:0;height:'+dh+'px;margin:0 1px;text-align:right;z-index:24;"><input id="'+fname+'['+uid+']" name="'+fname+'['+uid+']" class="fmftx" type="text" value="'+fxf_fn_float2string(value)+'" ref="fx_ta_425" style="margin-top:1px;width:'+(width-24)+'px;height:'+(dh-9)+'px;text-align:right;'+cs+'">&nbsp;<font class="fmfro">%</font></div>';
		// ...Text
		else
			div.innerHTML='<div id="d_'+fname+'['+uid+']" style="position:relative;top:0;left:0;height:'+dh+'px;margin:0 1px;z-index:24;"><input id="'+fname+'['+uid+']" name="'+fname+'['+uid+']" class="fmftx" type="text" value="'+value+'" ref="fx_ta_425" style="margin-top:1px;width:'+(width-9)+'px;height:'+(dh-9)+'px;"></div>';
	}
	// Textarea
	else if(dtype == 7)
	{
		div.innerHTML='<div id="d_'+fname+'['+uid+']" style="position:relative;top:0;left:0;height:'+dh+'px;margin:0 1px;z-index:24;"><textarea id="'+fname+'['+uid+']" name="'+fname+'['+uid+']" class="fmfta" ref="fx_ta_425" style="width:'+(width-9)+'px;height:'+(dh-9)+'px;resize:none;">'+value+'</textarea></div>';
	}
	// Select
	else if(dtype == 8)
	{
		var tx=fxf_fn_selectedValue(fname,value,true);
		var so=fxf_fn_getSelStyle(tx);
		var md='';
		if(Prototype.BrowserFeatures['Mobile'].length)
			md=' onclick="fxf_eh_mouseDown();"';
		div.innerHTML='<span style="float:right;"><div id="'+fname+'['+uid+']" name="'+fname+'['+uid+']" class="fmfsl" svalue="'+value+'" bg="'+so.bg+'" style="width:'+(width-8)+'px;'+so.st+'"'+md+'>'+so.tx+'<div class="fmfslc" style="cursor:pointer;"><span class="fmfsli"></span></div></div></span>';
	}
	// Checkbox
	else if(dtype == 11)
	{
		var cks='';
		if(value)
			cks=' checked';
		div.innerHTML='<div id="d_'+fname+'['+uid+']" style="position:relative;top:2px;left:0;text-align:center;z-index:24;"><input id="'+fname+'['+uid+']" name="'+fname+'['+uid+']" class="fmfcb" type="checkbox" value="1" ref="fx_ta_425"'+cks+'></div>';
	}
	// ReadOnly
	else
	{
		var ch=dh;
		var tp=4;
		var cs=col;
		// Alignment
		var talign='left';
		// ...Decimal, Budget, Duration, Costs per Hour or Percentage
		if((ftype == 6) || (ftype == 11) || (ftype == 16) || (ftype == 17) || (ftype == 50) || (ftype == 51) || (ftype == 52))
		{
			talign='right';
			if(value < 0.0)
				cs='color:#e87986;';
			else if(value == 0.0)
				cs='color:#b4b4b4;';
		}
		// ...Date/Time
		else if((ftype == 7) || (ftype == 9) || (ftype == 10))
			talign='center';
		else if(align.length)
			talign=align;
		// Style
		var sstyle=cs;
		if(talign != 'center')
			sstyle += 'float:'+talign+';padding:0 4px;';
		if(sstyle.length)
			sstyle=' style="'+sstyle+'"';
		var dstyle='';
		// Text
		// ...Decimal
		if(ftype == 6)
			value=fxf_fn_float2string(value);
		// ...Date/Time
		else if((ftype == 7) || (ftype == 9) || (ftype == 10))
		{
			if(value.substr(0,1) == '9')
			{
				if(oFXP.lang == 1)
					value='<i>(offen)</i>';
				else
					value='<i>(open)</i>';
			}
			else if(value.length)
			{
				if(value.length > 8)
					var ti=value.substr(8,2)+':'+value.substr(10,2);
				else
					var ti='00:00';

				if(ftype == 9)	// Time
					value=ti;
				else
				{
					value=fxf_fn_dateFormat(value);
					if(ftype == 10)	// DateTime
						value += ' '+ti;
				}
			}
		}
		// ...Budget
		else if((ftype == 11) || (ftype == 16) || (ftype == 17))
			value=fxf_fn_float2string(value)+'&nbsp;<font class="fmfro" style="padding-left:3px;">'+oSet.currency+'</font>';
		// ...Duration
		else if(ftype == 50)
			value=fxf_fn_sec2unit(value,-1)+'&nbsp;<font class="fmfro" style="padding-left:3px;">'+oSet.unitlit[gSet.dunit]+'</font>';
		// ...Costs per Hour
		else if(ftype == 51)
			value=fxf_fn_float2string(value)+'&nbsp;<font class="fmfro" style="padding-left:3px;">'+oSet.currency+'/'+oSet.unitlit[2]+'</font>';
		// ...Percentage
		else if(ftype == 52)
			value=fxf_fn_float2string(value)+'&nbsp;<font class="fmfro" style="padding-left:3px;">%</font>';
		// ...Select
		else if(ftype == -1)
		{
			var tx=fxf_fn_selectedValue(fname,value,true);
			var so=fxf_fn_getSelStyle(tx);
			value='<font style="padding-left:2px;">'+so.tx+'</font>';
			if(so.st.length)
			{
				dstyle += so.st;
				if((fname == 'pcat') && (so.st.substr(0,12) == 'background:#'))
					sstyle += ' bg="'+so.st.substr(11,7)+'"';
				dstyle += 'padding-top:4px;';
				tp=0;
				ch -= 6;
			}
		}
		// ...Checkbox
		else if(ftype == -11)
		{
			var cks='';
			if(value)
				cks=' checked';
			value='<input class="fmfcb" type="checkbox" value="1" disabled'+cks+'>';
		}
		div.innerHTML='<div id="d_'+fname+'['+uid+']" style="position:relative;left:0;top:'+tp+'px;right:0;height:'+ch+'px;text-align:'+talign+';z-index:24;'+dstyle+'"><span id="'+fname+'['+uid+']" name="'+fname+'['+uid+']" class="fmfro"'+sstyle+'>'+value+'</span></div>';
	}
}

function fxf_fn_markContextMenu(mark, event)
{
	var element=$(Event.element(event));
	if(!element || !element.id)
		return;

	var pelement=element.up();
	while(pelement.nodeName != 'TR')
		pelement=pelement.up();

	var celements=pelement.childElements();
	if(celements.length)
	{
		for(var c=0; c<celements.length; c++)
		{
			if(mark)
			{
				if(c == 0)
					celements[c].style.background='url(GFX/cm_mk.png) no-repeat left top';
				else if(c == celements.length-1)
					celements[c].style.background='url(GFX/cm_mk.png) no-repeat right top';
				else
					celements[c].style.background='url(GFX/cm_mkm.png) repeat left top';
			}
			else
				celements[c].style.background='transparent';
		}
	}
}

function fxf_fn_uploader(umode, multi, fieldname, htext)
{
//alert('fxf_fn_uploader(umode='+umode+', multi='+multi+', fieldname='+fieldname+', htext='+htext+')');
	if(umode < 0)
	{
		if(oID.oUploader.uploader.style.display == 'none')
			umode=1;
		else
			umode=0;
	}

	// Show uploader
	if(umode > 0)
	{
		// Block screen
		oID.iainfo.innerHTML='';
		oID.iainfo.style.display='none';
		oID.iact.style.display='';

		// Reset uploader parameters
		oID.oUploader.filelist=[];
		oID.oUploader.totalsize=0;

		// Set multi
		oID.oUploader.multi=multi;

		// Header
		oID.oUploader.uploader_header_text.innerHTML=htext;

		// Set form parameters
		oID.oUploader.uploader_select.value='';
		if(multi)
		{
			oID.oUploader.uploader_select.name=fieldname+'[]';
			if(!oID.oUploader.uploader_select.attributes || !oID.oUploader.uploader_select.attributes['multiple'])
				oID.oUploader.uploader_select.setAttribute('multiple','multiple');
			if(oFXP.lang == 1)
			{
				oID.oUploader.uploader_select_text.innerHTML='Datei(en) auswählen:';
				oID.oUploader.uploader_zone_text.innerHTML='Datei(en) zum Uploaden hierher ziehen...';
			}
			else
			{
				oID.oUploader.uploader_select_text.innerHTML='Select file(s):';
				oID.oUploader.uploader_zone_text.innerHTML='Move file(s) here to upload...';
			}
		}
		else
		{
			oID.oUploader.uploader_select.name=fieldname;
			if(oID.oUploader.uploader_select.attributes && oID.oUploader.uploader_select.attributes['multiple'])
				oID.oUploader.uploader_select.removeAttribute('multiple');
			if(oFXP.lang == 1)
			{
				oID.oUploader.uploader_select_text.innerHTML='Datei auswählen:';
				if((oFXP.tr == 110) && !multi)
					oID.oUploader.uploader_zone_text.innerHTML='Neue Version der Datei zum Uploaden hierher ziehen...';
				else
					oID.oUploader.uploader_zone_text.innerHTML='Datei zum Uploaden hierher ziehen...';
			}
			else
			{
				oID.oUploader.uploader_select_text.innerHTML='Select file:';
				if((oFXP.tr == 110) && !multi)
					oID.oUploader.uploader_zone_text.innerHTML='Move new version of the file here to upload...';
				else
					oID.oUploader.uploader_zone_text.innerHTML='Move file here to upload...';
			}
		}

		// Set hidden fields
		oID.oUploader.uploaderh_frm.value='';
		oID.oUploader.uploaderh_lts.value=oFXP.lts;
		oID.oUploader.uploaderh_upn.value=fieldname;
		oID.oUploader.uploaderh_div.value='';

		// Display uploader
		oID.oUploader.uploader.style.display='';

		// Center uploader
		var uw=oID.oUploader.uploader.clientWidth;
		var uh=oID.oUploader.uploader.clientHeight;
//alert('dim.sd.pwidth='+dim.sd.pwidth+', dim.sd.pheight='+dim.sd.pheight+' -- uw='+uw+', uh='+uh);
		oID.oUploader.uploader.style.left=Math.ceil((dim.sd.pwidth-uw)/2)+'px';
		oID.oUploader.uploader.style.top=Math.ceil((dim.sd.pheight-uh)/2)+'px';
	}
	// Hide uploader
	else
		fxf_fn_fxLinkClose();
}

function fxf_fn_dmsViewer(mode, apst)
{
	mode=mode.substr(0,3);
	fxf_fn_waitScreen('#');

	if(mode == 'ddl')
	{
		fxf_fn_waitScreen('');
		var url=fxf_fn_gProgram('dms_action', 'mode='+mode+'&submode=0&dpath='+fxf_fn_escapeText(apst,false)+'&pwidth='+dim.sd.pwidth+'&pheight='+dim.sd.pheight+fxf_fn_gParam());
//alert('fxf_fn_dmsViewer:\nmode: '+mode+'\nurl: '+url);
		window.location=url;
		return;
	}

	var url=fxf_fn_gProgram('dms_action', 'mode='+mode+'&submode=0')+'&pwidth='+dim.sd.pwidth+'&pheight='+dim.sd.pheight+fxf_fn_gParam();
//alert('fxf_fn_dmsViewer:\nmode: '+mode+'\napst: '+apst+'\nurl: '+url);
	new Ajax.Request(url,
	{
		method:'post', postBody:apst, asynchronous:asajax,
		onSuccess: function(transport)
		{
			var rt=transport.responseText;
//alert('rt:\n'+rt);
			if(rt.length)
			{
				var dtitle=apst.substr(6+apst.indexOf('sname='));

				if(mode == 'dsw')
				{
//alert('dsw: '+dtitle+'\n\nscreen.availLeft='+screen.availLeft);
					var rs=rt.split('|');
					var iframe='<iframe id="fm_display_iframe" src="'+rs[0]+'#'+fxf_fn_today(true)+'" width="100%" height="100%" marginwidth="0" marginheight="0" hspace="0" vspace="0" frameborder="0" style="border:0;margin:0;padding:0;"></iframe>';
					var dsw=window.open('','_blank','left='+(parseInt(rs[1])+screen.availLeft)+',top='+parseInt(rs[2])+',width='+parseInt(rs[3])+',height='+parseInt(rs[4])+',toolbar=no,location=no,directories=no,status=no,menubar=no,scrollbars=yes,copyhistory=no,resizable=yes');
					dsw.document.write('<html><head><title>'+dtitle+'</title></head><body>'+iframe+'</body></html>');
					fxf_fn_waitScreen('');
					dsw.focus.delay(oFXP.sdelay);
				}
				else
					fxf_fn_fxLinkDisplay(rt, dtitle);
			}
			else
				fxf_fn_waitScreen('');
		},
		onFailure: function()
		{
//alert('FAILURE');
			fxf_fn_showMessage('100', 'FAILURE: Can not view document!');
			fxf_fn_waitScreen('');
		}
	});
}

function fxf_fn_updateSelectedProjects(field, multi, use_actual, start_date, end_date, customers, persons, with_all, with_projects)
{
//alert('updateSelectedProjects(\nfield='+field+'\n\nuse_actual='+use_actual+'\n\nstart_date='+start_date+', end_date='+end_date+'\ncustomers='+customers+'\npersons='+persons+'\n\nwith_all='+with_all+', with_projects='+with_projects);

	fxf_fn_waitScreen('#');

	// Change project select element
	var pse=fxf_fn_getField($(field),true);
	if(pse)
	{
		var frm='';
		if(pse.attributes && (typeof pse.attributes['fxform'] != 'undefined'))
			frm=pse.attributes['fxform'].value;
		var opid=parseInt(pse.fxv);
		var url=fxf_fn_gProgram('sprojects', 'frm='+frm+'&s_name='+field+'&s_val='+opid);
		url += '&multi='+parseInt(multi)+'&use_actual='+parseInt(use_actual)+'&start_date='+start_date+'&end_date='+end_date+'&customers='+customers+'&persons='+persons+'&with_all='+parseInt(with_all)+'&with_projects='+parseInt(with_projects);
		url += fxf_fn_gParam();
//alert('url='+url);
		new Ajax.Request(url,
		{
			method:'get', asynchronous:true,
			onSuccess: function(transport)
			{
				var ret=transport.responseText;
//alert('ret='+ret);
				if((ret.length > 5) && (ret.substr(0,6) == '<input'))	// SelPopPType?
				{
					var eip=ret.indexOf('>');
					ret=ret.substr(eip+1);
				}

				if(multi)
				{
					pse.innerHTML=ret;
					fxf_fn_waitScreen('');
				}
				else
				{
					fxf_fn_setNewSelects(pse,ret,true);
					fxf_fn_waitScreen('');
				}
			}
		});
	}
	else
		fxf_fn_waitScreen('');
}

function fxf_fn_sleep(ms)
{
	return new Promise(resolve => setTimeout(resolve, ms));
}

function fxf_fn_getText(tid)
{
	var text='';

	if((tid > 0) && (aTXT.length > tid))
		text=aTXT[tid];
//alert('fxf_fn_getText('+tid+')\n'+text);

	return text;
}

function fxf_fn_dispLang(disp)
{
	var llang=$('log_lang');
	if(llang || !disp)
	{
		oID.fxtaskl.style.display='none';
		oID.fxtaskli.style.display='none';
	}
	else
	{
		oID.fxtaskl.style.display='';
		oID.fxtaskli.style.display='';
	}
}