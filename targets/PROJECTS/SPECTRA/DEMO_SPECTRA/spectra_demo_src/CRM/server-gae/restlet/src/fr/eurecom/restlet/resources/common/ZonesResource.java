package fr.eurecom.restlet.resources.common;

import java.util.List;

import org.json.JSONArray;
import org.json.JSONObject;
import org.mortbay.log.Log;
import org.restlet.Context;
import org.restlet.Request;
import org.restlet.Response;
import org.restlet.data.MediaType;
import org.restlet.data.Method;
import org.restlet.data.Reference;
import org.restlet.data.Status;
import org.restlet.ext.json.JsonRepresentation;
import org.restlet.ext.wadl.MethodInfo;
import org.restlet.ext.wadl.RepresentationInfo;
import org.restlet.ext.wadl.ResponseInfo;
import org.restlet.ext.wadl.WadlServerResource;
import org.restlet.ext.xml.DomRepresentation;
import org.restlet.representation.Representation;
import org.restlet.representation.StringRepresentation;
import org.restlet.resource.Get;
import org.w3c.dom.Document;
import org.w3c.dom.Element;

import fr.eurecom.senml.entity.IZone;
import fr.eurecom.senml.entity.ZoneAdmin;
import fr.eurecom.senml.persistence.JDOStorage;

public class ZonesResource extends WadlServerResource {

	public static enum ZoneFormat {
		ID("id", "zone identifier"), NAME("name", "zone name"), SUBZONE(
				"subzone", "subzone description"),
		// SENSORS("sensors","sensors lsit uri"),
		PARENT_ZONE("parent", "name zone parent"), ZONE("zone", "Zone"), URI(
				"uri", "URI"), HAS_SUBZONES("has_subzones", "Has subzones"), HAS_SENSORS(
				"has_sensors", "Has_sensors"), SENSORS_SENML("senml",
				"sensors over senml"),ZONE_TYPE("type", "type of the zone");

		private final String restAttr;
		private final String desc;

		ZoneFormat(String restAttr, String desc) {
			this.restAttr = restAttr;
			this.desc = desc;
		}

		public String getRestAtr() {
			return restAttr;
		}

		public String getDescription() {
			return desc;
		}
	};

	/**
	 * {@inheritDoc}
	 */
	@Override
	public Representation describe() {
		setName("Zones Resource");
		setDescription("Retrieve zones list");
		return super.describe();
	}

	/**
	 * {@inheritDoc}
	 */
	@Override
	public void describeGet(MethodInfo mInfo) {
		mInfo.setIdentifier("Zones List");
		Log.info("oooooo");
		mInfo.setDocumentation("To Retrieve a list of zones");
		ResponseInfo response = new ResponseInfo("Current list of zones");
		response.getStatuses().add(Status.SUCCESS_OK);

		RepresentationInfo repInfo = new RepresentationInfo(
				MediaType.APPLICATION_JSON);
		repInfo.setDocumentation("JSON array of of the current list zone");
		response.getRepresentations().add(repInfo);

		mInfo.getResponses().add(response);
	}

	// HACK: Version Restlet-gae 2.1-rc5.
	// There is an error routing the GET request when
	// the Request Header Content-Type = application/x-www-form-urlencoded and
	// Header Accept = "application/json" and
	// URI have a query : http://localhost:8888/contacts?sort=firstname.
	// In this case the router ignores the Accept header and route the request
	// to @Get("xml") systematically because the query is badly interpreted.
	// To workaround this problem, at the init we reformat the Entity with an
	// empty Representation.

	@Override
	public void init(Context context, Request request, Response response) {
		// System.out.println("init" + request + " " + response);
		// System.out.println("Content-Type: " +
		// request.getEntity().getMediaType());
		if (request.getMethod().compareTo(Method.GET) == 0) {
			Representation entity = request.getEntity();
			System.out.println("Entity " + entity + " Content-Type: "
					+ request.getEntity().getMediaType());
			request.setEntity(new StringRepresentation(""));
		}
		super.init(context, request, response);
	}

	/**
	 * Handle HTTP GET method / Json
	 * 
	 * @return a JSON list of root zones by default.
	 */
	@Get("json")
	public Representation toJSON() {
		// System.out.println("Request Original Ref " + getOriginalRef());
		// System.out.println("Request Entity " + getRequest().getEntityAsText()
		// +
		// " entity mediaType " + getRequest().getEntity().getMediaType());

		try {
			List<IZone> zones = JDOStorage.getInstance()
					.getAll(ZoneAdmin.class);
			JSONArray jzones = new JSONArray();
			Reference ref = getRequest().getResourceRef();
			final String baseURL = ref.getHierarchicalPart();

			for (IZone zone : zones) {
				if (zone.getParentZone() == null) {

					JSONObject jzone = new JSONObject();
					jzone.put(ZoneFormat.URI.getRestAtr(),
							String.format("%s/%s", baseURL, zone.getKey()));

					jzone.put(ZoneFormat.NAME.getRestAtr(), zone.getName());
					jzone.put(ZoneFormat.ZONE_TYPE.getRestAtr(), zone.getType());

					jzone.put(ZoneFormat.HAS_SENSORS.getRestAtr(), zone
							.getSensors().size() > 0);

					jzone.put(ZoneFormat.HAS_SUBZONES.getRestAtr(), zone
							.getSubZones().size() > 0);

					jzones.put(jzone);
				}
			}

			JSONObject zoneHead = new JSONObject();
			zoneHead.put(ZoneFormat.SUBZONE.getRestAtr(), jzones);
			return new JsonRepresentation(zoneHead);
		} catch (Exception e) {
			setStatus(Status.SERVER_ERROR_INTERNAL);
			return null;
		}
	}

	@Get("xml")
	public Representation toXML() {
		try {
			List<IZone> zones = JDOStorage.getInstance()
					.getAll(ZoneAdmin.class);
			Reference ref = getRequest().getResourceRef();
			final String baseURL = ref.getHierarchicalPart();

			DomRepresentation xml = new DomRepresentation(MediaType.TEXT_XML);
			Document d = xml.getDocument();
			Element root = d.createElement(ZoneFormat.ZONE.getRestAtr());
			d.appendChild(root);

			for (IZone zone : zones) {
				if (zone.getParentZone() == null) {
					Element subzone = d.createElement(ZoneFormat.SUBZONE
							.getRestAtr());

					subzone.setAttribute(ZoneFormat.NAME.getRestAtr(),
							zone.getName());

					subzone.setAttribute(ZoneFormat.URI.getRestAtr(),
							String.format("%s/%s", baseURL, zone.getKey()));

					subzone.setAttribute(ZoneFormat.HAS_SENSORS.getRestAtr(),
							"" + (zone.getSensors().size() > 0));

					subzone.setAttribute(ZoneFormat.HAS_SUBZONES.getRestAtr(),
							"" + (zone.getSubZones().size() > 0));
				root.appendChild(subzone);	
				}
			}

			return xml;
		} catch (Exception e) {
			e.printStackTrace();
			setStatus(Status.SERVER_ERROR_INTERNAL);
			return null;
		}
	}
}