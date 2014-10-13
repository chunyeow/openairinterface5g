package fr.eurecom.senml.jspcontroller;

import java.util.Calendar;
import java.util.List;
import java.util.TimeZone;
import java.util.concurrent.TimeUnit;

import javax.servlet.http.HttpServletRequest;

import org.springframework.stereotype.Controller;
import org.springframework.ui.ModelMap;
import org.springframework.web.bind.annotation.ModelAttribute;
import org.springframework.web.bind.annotation.PathVariable;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RequestMethod;
import org.springframework.web.servlet.ModelAndView;

import fr.eurecom.pushing.GCMPushingServer;
import fr.eurecom.senml.entity.IPostVal;
import fr.eurecom.senml.entity.IZone;
import fr.eurecom.senml.entity.Measure;
import fr.eurecom.senml.entity.PostValAdmin;
import fr.eurecom.senml.entity.PostValType;
import fr.eurecom.senml.entity.Sensor;
import fr.eurecom.senml.entity.SensorAdmin;
import fr.eurecom.senml.entity.SensorCheckAlarm;
import fr.eurecom.senml.entity.Units;
import fr.eurecom.senml.entity.Zone;
import fr.eurecom.senml.entity.ZoneAdmin;
import fr.eurecom.senml.persistence.JDOStorage;

@Controller
@RequestMapping("/")
public class ControllerJsp {
	
	@RequestMapping(value = "/zones", method = RequestMethod.GET)
//	@ModelAttribute
	public String listZone(ModelMap model) {
		List<IZone> zones = JDOStorage.getInstance().getAll(ZoneAdmin.class);
		model.addAttribute("zoneList", zones);
		return "zonelist";
	}
	
	@RequestMapping(value="/addZone", method = RequestMethod.POST)
	public ModelAndView addZone(HttpServletRequest request) {
		String name = request.getParameter("nameZone"); 
		String nameParent = request.getParameter("parentNameZone");
		String type = request.getParameter("typeZone");
		
		ZoneAdmin parent = nameParent.isEmpty() 
				? null 
				:JDOStorage.getInstance().getById(nameParent, ZoneAdmin.class);

		new ZoneAdmin(name, parent, type);
		return new ModelAndView("redirect:zones");
	}
	
	@RequestMapping(value = "deleteZone/{zoneKey}", method = RequestMethod.GET) 
	public ModelAndView deleteZone(@PathVariable String zoneKey, HttpServletRequest request) {
		
		ZoneAdmin z = JDOStorage.getInstance().getById(zoneKey, ZoneAdmin.class);
		if (z != null) {
			z.destroy();
		}
		return new ModelAndView("redirect:../zones");
	}
	

	@RequestMapping(value="{zoneKey}/edit", method = RequestMethod.GET)
	public String editZone(@PathVariable String zoneKey, HttpServletRequest request, 
			ModelMap model) {
		ZoneAdmin z = JDOStorage.getInstance().getById(zoneKey, ZoneAdmin.class);
		model.addAttribute("zone", z);
		return "detailzone";
	}
	
	
	@RequestMapping(value="{zoneKey}/addsubZone", method = RequestMethod.POST)
	public String addSubzone(@PathVariable String zoneKey, HttpServletRequest request, ModelMap model) {
		String subZone = request.getParameter("nameSubZone");
		String typeZone = request.getParameter("typeZone");
		
		ZoneAdmin zone = JDOStorage.getInstance().getById(
				zoneKey, ZoneAdmin.class);

		new ZoneAdmin(subZone, zone, typeZone);
		model.addAttribute("zone", zone);
		return "redirect:edit";
	}
	
	@RequestMapping(value="{zoneKey}/addSensor", method = RequestMethod.POST) 
	public String addSensor(@PathVariable String zoneKey, HttpServletRequest request, ModelMap model) {
		String title = request.getParameter("title");
		String baseUnit = request.getParameter("baseunit");
		String typeSensor = request.getParameter("typeSensor");
		ZoneAdmin za = JDOStorage.getInstance().getById(zoneKey, ZoneAdmin.class);
		new SensorAdmin(title, za, baseUnit, typeSensor);
		
		model.addAttribute("zone", za);
		return "redirect:edit";
	}
	
	@RequestMapping(value= "{zoneKey}/deleteSensor/{sensorKey}", method = RequestMethod.GET)
	public ModelAndView deleteSensor(@PathVariable String zoneKey,  
			@PathVariable String sensorKey, HttpServletRequest request, ModelMap model) {
		
		SensorAdmin s = JDOStorage.getInstance().getById(sensorKey, SensorAdmin.class);
		s.destroy();
		
		return new ModelAndView("redirect:../edit");
	}	
	
	
	@RequestMapping(value="{zoneKey}/delete/{subZoneKey}", method = RequestMethod.GET)
	@ModelAttribute
	public ModelAndView deleteZone(@PathVariable String zoneKey, @PathVariable String subZoneKey,
            HttpServletRequest request, ModelMap model) {
		ZoneAdmin z = JDOStorage.getInstance().getById(subZoneKey, ZoneAdmin.class);
		if (z != null) {
			z.destroy();
		}
		
		return new ModelAndView("redirect:../edit"); 		
	}
	
	@RequestMapping(value="{zoneKey}/addMeasure", method = RequestMethod.POST)
	public String addMeasure(@PathVariable String zoneKey, 
			HttpServletRequest request, ModelMap model) {	
		
		// Mandatory fields
		String sensorKey = request.getParameter("sensorKey");
		String unit = request.getParameter("unit");
		String name = request.getParameter("name");
		String typeValue = request.getParameter("typeValue");
		String value = request.getParameter("value");
		long time = getSecondsSince1970UTC(request.getParameter("time"));
		String strVersion = request.getParameter("version");
		// Optional values
		String sumValue = request.getParameter("sum");
		String specMeasure = request.getParameter("specmeasure");
		if (specMeasure != null) System.out.println("spec measure: " + specMeasure.toString());
		String baseUnit = request.getParameter("bu");
		String allowed = request.getParameter("allowed");
		String rtallowed = request.getParameter("rtallowed");
		String type = request.getParameter("measureType");
		
		// If version is not setted, automatically is 1
		if (strVersion == null) {
			strVersion = "1.0";
		}
		
		float version = Float.parseFloat(strVersion);
		
		SensorAdmin sa = JDOStorage.getInstance().getById(sensorKey, SensorAdmin.class);
		
		if (sumValue == null) {
			if ((specMeasure == null) || (specMeasure.equalsIgnoreCase("off"))) {
				System.out.println("using baseUnit " + baseUnit);
				sa.addMeasure(
						name,
						null,
						Measure.ParamTypeValue.valueOf(typeValue.toUpperCase()),
						value, time, type);
			} else {
				System.out.println("specMeasure is " + specMeasure);
				sa.addMeasure(
						name,
						Units.getUnit(unit),
						Measure.ParamTypeValue.valueOf(typeValue.toUpperCase()),
						value, time, type);
			}
		} else {
			if ((specMeasure == null) || (specMeasure.equalsIgnoreCase("off"))) {
				System.out.println("using baseUnit " + baseUnit);
				sa.addMeasure(
						name,
						null,
						Measure.ParamTypeValue.valueOf(typeValue.toUpperCase()),
						value, time, type, sumValue);
			} else {
				System.out.println("specMeasure is " + specMeasure);
				sa.addMeasure(
						name,
						Units.getUnit(unit),
						Measure.ParamTypeValue.valueOf(typeValue.toUpperCase()),
						value, time, type, sumValue);
			}
			
		}
		
		model.addAttribute("zone", sa.getZone());
		// Eventually notify the registered devices
		
		return "redirect:edit";
	}
	
	@RequestMapping(value="{zoneKey}/addAlertMonitor", method = RequestMethod.POST)
	public String addAlertMonitor(@PathVariable String zoneKey, 
			HttpServletRequest request, ModelMap model) {	
		
		// Mandatory fields
		String sensorKey = request.getParameter("sensorKey");
		float rtallowedmin = Float.parseFloat(request.getParameter("rtallowedmin"));
		float rtallowedmax = Float.parseFloat(request.getParameter("rtallowedmax"));
		
		ZoneAdmin zone = JDOStorage.getInstance().getById(zoneKey, ZoneAdmin.class);
		Sensor sensor = JDOStorage.getInstance().getById(sensorKey, SensorAdmin.class);
		System.out.println("Adding monitoring for " + zone.getName() + ", " + sensor.getTitle());
		
		SensorCheckAlarm checkAlarm = new SensorCheckAlarm(sensorKey, rtallowedmin, rtallowedmax);
		JDOStorage.getInstance().write(checkAlarm);
		return "redirect:edit";
	}
	
	// Admin website version - TODO, filter for sensors/zones
	@RequestMapping(value="{zoneKey}/listAlertsMonitor", method = RequestMethod.GET)
	public String listAlertsMonitor(@PathVariable String zoneKey, 
			HttpServletRequest request, ModelMap model) {	
		//TODO IF NEEDED!!!!
		return "alarms";
	}
	
	// Admin website version for all the monitors
	@RequestMapping(value="/listAllAlertsMonitorJSON", method = RequestMethod.GET)
	public String listAllAlertsMonitorJSON(HttpServletRequest request, ModelMap model) {	
		return "allalarmsJSON";
	}
	
	// Admin mobile app version
	@RequestMapping(value="{zoneKey}/listAlertsMonitorJSON", method = RequestMethod.GET)
	public String listAlertsMonitorJSON(@PathVariable String zoneKey, 
			HttpServletRequest request, ModelMap model) {	
		return "alarmsJSON";
	}
	
	@RequestMapping(value="{zoneKey}/removemonitor", method = RequestMethod.GET)
	public String removeMonitor(@PathVariable String zoneKey, 
			HttpServletRequest request, ModelMap model) {
		String sensorKey = request.getParameter("key");
		SensorCheckAlarm s = JDOStorage.getInstance().getById(sensorKey, SensorCheckAlarm.class);
		JDOStorage.getInstance().delete(s);
		return "alarms";
	}
	
	@RequestMapping(value="{zoneKey}/getmonitor", method = RequestMethod.GET)
	public String getMonitor(@PathVariable String zoneKey, 
			HttpServletRequest request, ModelMap model) {
		return "sensormonitorsjson";
	}
	
	/**
	 * To respect the senML directives, we need seconds since 1970 UTC
	 * @param parameter
	 * @return
	 */
	private long getSecondsSince1970UTC(String parameter) {
		Calendar calendar = Calendar.getInstance(TimeZone.getTimeZone("UTC"));
		System.out.println("Calendar is: " + calendar);
		int secondsSinceEpoch = (int) TimeUnit.MILLISECONDS.toSeconds(calendar.getTimeInMillis());
		System.out.println("seconds are " + secondsSinceEpoch);
		return secondsSinceEpoch;
	}

	@RequestMapping(value= "{zoneKey}/deleteMeasure/{keyMeasure}", 
			method = RequestMethod.GET)
	public ModelAndView deleteMeasure(@PathVariable String zoneKey, @PathVariable String keyMeasure, 
			HttpServletRequest request, ModelMap model) {
		
		Measure m = JDOStorage.getInstance().getById(keyMeasure, Measure.class);
//		m.destroy();
		SensorAdmin sa = JDOStorage.getInstance().getById(m.getSensorKey(), SensorAdmin.class);
		sa.deleteMeasure(m);
		return new ModelAndView("redirect:../edit");
	}
	
	@RequestMapping(value = "/postvals", method = RequestMethod.GET)
	public String listPostVals(ModelMap model) {
		List<PostValType> zones = JDOStorage.getInstance().getAll(PostValType.class);
		model.addAttribute("postvalsList", zones);
		return "postvalslist";
	}
//	
//	@RequestMapping(value = "/alertsadmin", method = Request.GET)
//	public String listMonitoredSensors()
}
