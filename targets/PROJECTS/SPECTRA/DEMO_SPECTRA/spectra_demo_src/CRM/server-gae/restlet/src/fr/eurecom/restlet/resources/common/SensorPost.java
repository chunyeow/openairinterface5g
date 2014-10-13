package fr.eurecom.restlet.resources.common;

import java.math.BigInteger;
import java.security.MessageDigest;
import java.util.Date;
import java.util.List;
import java.util.logging.Level;
import java.util.logging.Logger;

import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;
import org.restlet.data.MediaType;
import org.restlet.data.Reference;
import org.restlet.data.Status;
import org.restlet.data.Tag;
import org.restlet.ext.json.JsonRepresentation;
import org.restlet.ext.wadl.MethodInfo;
import org.restlet.ext.wadl.RepresentationInfo;
import org.restlet.ext.wadl.ResponseInfo;
import org.restlet.ext.wadl.WadlServerResource;
import org.restlet.ext.xml.DomRepresentation;
import org.restlet.representation.Representation;
import org.restlet.resource.Get;
import org.restlet.resource.Post;
import org.w3c.dom.Document;
import org.w3c.dom.Element;
import org.w3c.dom.Node;

import fr.eurecom.restlet.resources.common.ZonesResource.ZoneFormat;
import fr.eurecom.senml.entity.ISensor;
import fr.eurecom.senml.entity.IZone;
import fr.eurecom.senml.entity.PostValAdmin;
import fr.eurecom.senml.entity.ZoneAdmin;
import fr.eurecom.senml.persistence.JDOStorage;

public class SensorPost extends WadlServerResource {
	private static final Logger log = Logger.getLogger("SensorPost");	

	/**
	 * {@inheritDoc}
	 */
	@Override
	public Representation describe() {
		setName("Sensor Post");
		setDescription("Post Something, is a test!");
		return super.describe();
	}
	
	@Post("json")
	public Representation doPost(String entity) {
		String senmlValue = (String) getRequest().getAttributes().get("senml");
		//IZone zone = JDOStorage.getInstance().getById(id, ZoneAdmin.class);
		JSONObject jzone = new JSONObject();
		try {
			jzone.put("senml", senmlValue);
		} catch (JSONException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		Date n = new Date();
		PostValAdmin sa = new PostValAdmin();
		sa.addPost("testing post " + String.valueOf(n),  
				entity,
				"NOW".equals(n) ? 0 : System.currentTimeMillis());
		
		Representation out = new JsonRepresentation(jzone);
		return out;

	}

	/**
	 * {@inheritDoc}
	 */
	@Override
	public void describeGet(MethodInfo mInfo) {
		mInfo.setIdentifier("Sensor Posting");
		mInfo.setDocumentation("To Retrieve a zone description");
		ResponseInfo response = new ResponseInfo("Current list of zones");
		response.getStatuses().add(Status.SUCCESS_OK);

		RepresentationInfo repInfo = new RepresentationInfo(
				MediaType.APPLICATION_JSON);
		repInfo.setDocumentation("JSON array of of the current list zone");
		response.getRepresentations().add(repInfo);

		mInfo.getResponses().add(response);
	}

	/**
	 * Handle HTTP GET method / Json
	 * 
	 * @return a JSON Zone Representation
	 */
	@Get("json")
	public Representation toJSON() {

		final String baseURL = buildBaseURL(getRequest().getResourceRef());

		String id = (String) getRequest().getAttributes().get("id");
		IZone zone = JDOStorage.getInstance().getById(id, ZoneAdmin.class);
		JSONObject jzone = new JSONObject();

		try {

			jzone.put(ZoneFormat.NAME.getRestAtr(), zone.getName());

			JSONArray subZones = new JSONArray();

			for (IZone sub : zone.getSubZones()) {
				JSONObject z = new JSONObject();
				z.put(ZoneFormat.URI.getRestAtr(),
						String.format("%s/%s", baseURL, sub.getKey()));
				z.put(ZoneFormat.NAME.getRestAtr(), sub.getName());
				z.put(ZoneFormat.HAS_SENSORS.getRestAtr(), sub.getSensors()
						.size() > 0);

				z.put(ZoneFormat.HAS_SUBZONES.getRestAtr(), sub.getSubZones()
						.size() > 0);

				subZones.put(z);
			}
			if (subZones.length() > 0) {
				jzone.put(ZoneFormat.SUBZONE.getRestAtr(), subZones);
			}

			JSONArray sensors = new JSONArray();
			for (ISensor sensor : zone.getSensors()) {
				sensors.put(sensor.toJSONSenML());
			}
			if (sensors.length() > 0) {
				jzone.put(ZoneFormat.SENSORS_SENML.getRestAtr(), sensors);
			}

/*			String md5 = md5(jzone.toString());
			Tag tag = md5 == null ? null : new Tag(md5, false);
			if (!isModified(tag)) {
				setStatus(Status.REDIRECTION_NOT_MODIFIED);
				return null;
			}
*/
			Representation json = new JsonRepresentation(jzone);
//			json.setTag(tag);
			return json;
		} catch (Exception e) {
			setStatus(Status.SERVER_ERROR_INTERNAL);
			log.log(Level.WARNING, "", e);
			return null;
		}
	}

	/**
	 * HTTP GET method / xml accept
	 * 
	 * @return a XML Zone Representation
	 */

	@Get("xml")
	public Representation toXML() {
		long before = System.currentTimeMillis();
		String id = (String) getRequest().getAttributes().get("id");
		IZone zone = JDOStorage.getInstance().getById(id, ZoneAdmin.class);
		final String baseURL = buildBaseURL(getRequest().getResourceRef());

		try {
			DomRepresentation xml = new DomRepresentation(MediaType.TEXT_XML);
			Document d = xml.getDocument();

			Element currentZone = d.createElement(ZoneFormat.ZONE.getRestAtr());
			currentZone.setAttribute(ZoneFormat.NAME.getRestAtr(),
					zone.getName());
			d.appendChild(currentZone);

			for (IZone subzone : zone.getSubZones()) {
				Element sub = d.createElement(ZoneFormat.SUBZONE.getRestAtr());
				sub.setAttribute(ZoneFormat.NAME.getRestAtr(),
						subzone.getName());
				sub.setAttribute(ZoneFormat.URI.getRestAtr(),
						String.format("%s/%s", baseURL, subzone.getKey()));
				sub.setAttribute(ZoneFormat.HAS_SENSORS.getRestAtr(), ""
						+ (subzone.getSensors().size() > 0));

				sub.setAttribute(ZoneFormat.HAS_SUBZONES.getRestAtr(), ""
						+ (subzone.getSubZones().size() > 0));

				currentZone.appendChild(sub);

			}

			for (ISensor sensor : zone.getSensors()) {
				Node senml = sensor.toXMLSenML(d);
				currentZone.appendChild(senml);
			}

/*			String md5 = md5(xml.getText());
			Tag tag = md5 == null ? null : new Tag(md5, false);
			if (!isModified(tag)) {
				setStatus(Status.REDIRECTION_NOT_MODIFIED);
				return null;
			}
*/
//			xml.setTag(tag);
			return xml;
		} catch (Exception e) {
			log.log(Level.WARNING, "", e);
			setStatus(Status.SERVER_ERROR_INTERNAL);
			return null;
		}
	}

	/*
	 * Alternative method to add an header to the response. Series<Header>
	 * responseHeaders = (Series<Header>)
	 * getResponse().getAttributes().get(HeaderConstants.ATTRIBUTE_HEADERS); if
	 * (responseHeaders == null) { responseHeaders = new
	 * Series<Header>(Header.class); getResponse().getAttributes().put(
	 * HeaderConstants.ATTRIBUTE_HEADERS, responseHeaders);
	 * 
	 * } responseHeaders.add(new Header("ETag", md5));
	 */

	/**
	 * Build a String baseURL = to current URI/Reference less the last path
	 * segment.
	 * 
	 * @param reference
	 *            the current reference object
	 * @return String base URI.
	 */
	private String buildBaseURL(Reference reference) {
		Reference ref = new Reference(reference);
		List<String> segments = ref.getSegments();
		if (segments.size() > 0) {
			segments.remove(segments.size() - 1);
			ref.setSegments(segments);
		}
		return ref.getHierarchicalPart();
	}

	private String md5(String value) {
		try {
			MessageDigest m = MessageDigest.getInstance("MD5");
			m.reset();
			m.update(value.getBytes());
			byte[] digest = m.digest();
			BigInteger bigInt = new BigInteger(1, digest);
			String hashtext = bigInt.toString(16);
			// Now we need to zero pad it if you actually want the full 32
			// chars.
			while (hashtext.length() < 32) {
				hashtext = "0" + hashtext;
			}
			return hashtext;
		} catch (Exception e) {
			e.printStackTrace();
			return null;
		}
	}

	private boolean isModified(Tag currentTag) {
		boolean modified = true;
		if (currentTag == null) {
			return modified;
		}

		if (getRequest().getConditions() != null
				&& getRequest().getConditions().getNoneMatch() != null) {

			for (Tag noMatch : getRequest().getConditions().getNoneMatch()) {
				if (currentTag.equals(noMatch)) {
					modified = false;
					break;
				}
			}
		}
		return modified;
	}
}
