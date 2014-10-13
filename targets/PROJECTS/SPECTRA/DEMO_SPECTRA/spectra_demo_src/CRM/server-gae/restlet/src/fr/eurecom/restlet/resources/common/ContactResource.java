package fr.eurecom.restlet.resources.common;

import java.util.Map;

import org.json.JSONObject;
import org.restlet.data.Form;
import org.restlet.data.MediaType;
import org.restlet.data.Status;
import org.restlet.ext.json.JsonRepresentation;
import org.restlet.ext.wadl.MethodInfo;
import org.restlet.ext.wadl.ParameterInfo;
import org.restlet.ext.wadl.ParameterStyle;
import org.restlet.ext.wadl.RepresentationInfo;
import org.restlet.ext.wadl.RequestInfo;
import org.restlet.ext.wadl.ResponseInfo;
import org.restlet.ext.xml.DomRepresentation;
import org.restlet.representation.Representation;
import org.restlet.resource.Delete;
import org.restlet.resource.Get;
import org.restlet.resource.Put;
import org.restlet.resource.ResourceException;
import org.w3c.dom.Document;
import org.w3c.dom.Element;

/**
 * 
 * This class represents the Contact resource
 */

public class ContactResource extends ContactBaseResource {

	/**
	 * underlying contact.
	 */
	private Contact contact = null;

	/**
	 * Resource contact identifier.
	 */
	private int identifier = -1;

	@Override
	public Representation describe() {
		System.out.println("describe");
		setName("Contact Resource");
		setDescription("Manage the current Contact");
		return super.describe();
	}

	@Override
	public void describeDelete(MethodInfo mInfo) {
		mInfo.setIdentifier("ContactDelete");
		mInfo.setDocumentation("To Delete the current contact.");

		ResponseInfo response = new ResponseInfo(
				"Contact deleted. No contents returned.");
		response.getStatuses().add(Status.SUCCESS_NO_CONTENT);
		mInfo.getResponses().add(response);

		response = new ResponseInfo("Contact not found");
		response.getStatuses().add(Status.CLIENT_ERROR_NOT_FOUND);
		mInfo.getResponses().add(response);
	}

	@Override
	public void describeGet(MethodInfo mInfo) {
		mInfo.setIdentifier("ContactDetail");
		mInfo.setDocumentation("To retrieve details of the current contact.");

		ResponseInfo response = new ResponseInfo("Current contact details");
		response.getStatuses().add(Status.SUCCESS_OK);

		RepresentationInfo repInfo = new RepresentationInfo(
				MediaType.APPLICATION_XML);
		repInfo.setXmlElement(CONTACT);
		repInfo.setDocumentation("XML representation of the current contact.");
		response.getRepresentations().add(repInfo);

		repInfo = new RepresentationInfo(MediaType.APPLICATION_JSON);
		repInfo.setDocumentation("JSON representation of the current contact");
		repInfo.setXmlElement(CONTACT);
		response.getRepresentations().add(repInfo);
		mInfo.getResponses().add(response);

		response = new ResponseInfo("Contact not found");
		repInfo = new RepresentationInfo(MediaType.TEXT_HTML);
		repInfo.setIdentifier("contactError");
		response.getStatuses().add(Status.CLIENT_ERROR_NOT_FOUND);
		response.getRepresentations().add(repInfo);
		mInfo.getResponses().add(response);
	}

	@Override
	public void describePut(MethodInfo mInfo) {
		mInfo.setIdentifier("ContactUpdate");
		mInfo.setDocumentation("To update details of the current contact");

		RequestInfo requestInfo = new RequestInfo();
		// requestInfo.getParameters().add(
		// new ParameterInfo(ID, true, "Number", ParameterStyle.QUERY,
		// "Contact identifier"));
		requestInfo.getParameters().add(
				new ParameterInfo(FIRST_NAME, false, "String",
						ParameterStyle.QUERY, "Contact First Name"));
		requestInfo.getParameters().add(
				new ParameterInfo(LAST_NAME, false, "String",
						ParameterStyle.QUERY, "Contact Last Name"));
		requestInfo.getParameters().add(
				new ParameterInfo(PHONE, false, "String", ParameterStyle.QUERY,
						"Contact Phone number"));
		requestInfo.getParameters().add(
				new ParameterInfo(MAIL, false, "String", ParameterStyle.QUERY,
						"Contact email address"));

		mInfo.setRequest(requestInfo);

		ResponseInfo responseInfo = new ResponseInfo("Contact not Found");
		responseInfo.getStatuses().add(Status.CLIENT_ERROR_NOT_FOUND);
		mInfo.getResponses().add(responseInfo);

		responseInfo = new ResponseInfo(
				"Contact updated, no contents returned.");
		responseInfo.getStatuses().add(Status.SUCCESS_NO_CONTENT);
		mInfo.getResponses().add(responseInfo);
	}

	@Override
	protected void doInit() throws ResourceException {
		String sIdentifier = (String) getRequest().getAttributes().get("id");
		if (sIdentifier != null) {
			identifier = Integer.parseInt(sIdentifier);
			contact = getContact(identifier);
		}
		setExisting(contact != null);
	}

	/**
	 * Handle HTTP GET method / xml
	 * 
	 * @return a xml resource representation
	 */
	@Get("xml")
	public Representation toXML() {
		try {
			DomRepresentation representation = new DomRepresentation(
					MediaType.TEXT_XML);

			Document d = representation.getDocument();
			Element eltItem = d.createElement(CONTACT);
			d.appendChild(eltItem);

			Element el = d.createElement(ID);
			el.appendChild(d.createTextNode("" + contact.getId()));
			eltItem.appendChild(el);

			el = d.createElement(FIRST_NAME);
			el.appendChild(d.createTextNode(contact.getFirstName()));
			eltItem.appendChild(el);

			el = d.createElement(LAST_NAME);
			el.appendChild(d.createTextNode(contact.getLastName()));
			eltItem.appendChild(el);

			el = d.createElement(PHONE);
			el.appendChild(d.createTextNode(contact.getPhone()));
			eltItem.appendChild(el);

			el = d.createElement(MAIL);
			el.appendChild(d.createTextNode(contact.getMail()));
			eltItem.appendChild(el);

			d.normalizeDocument();

			return representation;
		} catch (Exception e) {
			e.printStackTrace();
			return null;
		}
	}

	/**
	 * Handle HTTP GET method / json
	 * 
	 * @return a json resource representation.
	 */
	@Get("json")
	public Representation toJSON() {
		JSONObject jObject = new JSONObject();
		try {
			jObject.put(ID, "" + contact.getId());
			jObject.put(FIRST_NAME, contact.getFirstName());
			jObject.put(LAST_NAME, contact.getLastName());
			jObject.put(PHONE, contact.getPhone());
			jObject.put(MAIL, contact.getMail());

			return new JsonRepresentation(jObject);
		} catch (Exception e) {
			return null;
		}
	}

	/**
	 * Handle HTTP PUT method. Update an existing contact.
	 * 
	 * @param entity
	 */
	@Put
	public void updateContact(Representation entity) {
		Map<String, String> map = new Form(entity).getValuesMap();

		for (String k : map.keySet()) {
			MEMBERS m = MEMBERS.valueOf(k.toUpperCase());

			switch (m) {
			case FIRSTNAME:
				contact.setFirstName(map.get(k));
				break;
			case LASTNAME:
				contact.setLastName(map.get(k));
				break;
			case PHONE:
				contact.setPhone(map.get(k));
				break;
			case MAIL:
				contact.setMail(map.get(k));
				break;
			}
		}

		Status updateStatus = super.updateContact(contact);
		setStatus(updateStatus);
	}

	/**
	 * Handle HTTP DELETE method. Delete an existing contact.
	 */
	@Delete
	public void deleteContact() {
		Status deleteStatus = super.removeContact(identifier);
		setStatus(deleteStatus);
	}
}
