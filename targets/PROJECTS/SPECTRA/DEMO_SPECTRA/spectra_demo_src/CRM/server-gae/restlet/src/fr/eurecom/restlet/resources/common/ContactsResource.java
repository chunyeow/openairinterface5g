package fr.eurecom.restlet.resources.common;

import java.util.Iterator;

import org.json.JSONArray;
import org.json.JSONObject;
import org.restlet.Context;
import org.restlet.Request;
import org.restlet.Response;
import org.restlet.data.Form;
import org.restlet.data.MediaType;
import org.restlet.data.Method;
import org.restlet.data.Reference;
import org.restlet.data.Status;
import org.restlet.ext.json.JsonRepresentation;
import org.restlet.ext.wadl.MethodInfo;
import org.restlet.ext.wadl.OptionInfo;
import org.restlet.ext.wadl.ParameterInfo;
import org.restlet.ext.wadl.ParameterStyle;
import org.restlet.ext.wadl.RepresentationInfo;
import org.restlet.ext.wadl.RequestInfo;
import org.restlet.ext.wadl.ResponseInfo;
import org.restlet.ext.xml.DomRepresentation;
import org.restlet.representation.Representation;
import org.restlet.representation.StringRepresentation;
import org.restlet.resource.Get;
import org.restlet.resource.Post;
import org.w3c.dom.Document;
import org.w3c.dom.Element;

public class ContactsResource extends ContactBaseResource {

	/**
	 * {@inheritDoc}
	 */
	@Override
	public Representation describe() {
		setName("Contacts Resource");
		setDescription("Retrieve contacts list");
		return super.describe();
	}

	/**
	 * {@inheritDoc}
	 */
	@Override
	public void describeGet(MethodInfo mInfo) {
		mInfo.setIdentifier("ContactsList");
		mInfo.setDocumentation("To Retrieve a sorted list of contacts");

		RequestInfo requestInfo = new RequestInfo();
		ParameterInfo pInfo = createCommonParameter(REQUEST_QUERY_SORT,
				"Sort query", LAST_NAME);
		pInfo.getOptions().add(
				createOptionInfo(FIRST_NAME, "Sort by first name"));
		pInfo.getOptions()
				.add(createOptionInfo(LAST_NAME, "Sort by last name"));
		pInfo.getOptions().add(createOptionInfo(MAIL, "Sort by Email"));
		pInfo.getOptions().add(createOptionInfo(PHONE, "Sort by Phone"));

		requestInfo.getParameters().add(pInfo);
		mInfo.setRequest(requestInfo);

		ResponseInfo response = new ResponseInfo("Current list of contacts");
		response.getStatuses().add(Status.SUCCESS_OK);

		RepresentationInfo repInfo = new RepresentationInfo(
				MediaType.APPLICATION_XML);
		repInfo.setXmlElement(CONTACTS);
		repInfo.setDocumentation("XML List of contacts");
		response.getRepresentations().add(repInfo);

		repInfo = new RepresentationInfo(MediaType.APPLICATION_JSON);
		repInfo.setXmlElement(CONTACTS);
		repInfo.setDocumentation("JSON List of contacts");
		response.getRepresentations().add(repInfo);

		mInfo.getResponses().add(response);
	}

	/**
	 * {@inheritDoc}
	 */
	@Override
	public void describePost(MethodInfo mInfo) {
		mInfo.setIdentifier("ContactsAdd");
		mInfo.setIdentifier("To add a new contact to the list");

		RequestInfo requestInfo = new RequestInfo();
		requestInfo.getParameters().add(
				new ParameterInfo(ID, true, "Number", ParameterStyle.QUERY,
						"Contact identifier"));
		requestInfo.getParameters().add(
				createCommonParameter(FIRST_NAME, "Contact First Name"));
		requestInfo.getParameters().add(
				createCommonParameter(LAST_NAME, "Contact Last Name"));
		requestInfo.getParameters().add(
				createCommonParameter(PHONE, "Contact Phone Number"));
		requestInfo.getParameters().add(
				createCommonParameter(MAIL, "Contact Email address"));

		mInfo.setRequest(requestInfo);

		ResponseInfo response = new ResponseInfo("Contact saved");
		response.getStatuses().add(Status.SUCCESS_CREATED);
		response.getParameters().add(
				new ParameterInfo("Content-Location", ParameterStyle.HEADER,
						"URI contact saved"));
		mInfo.getResponses().add(response);

		response = new ResponseInfo("Contact not saved. (restricted to 50)");
		response.getStatuses().add(Status.SERVER_ERROR_INSUFFICIENT_STORAGE);
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
	 * Handle HTTP GET Metod / xml
	 * 
	 * @return an XML list of contacts.
	 */
	@Get("xml")
	public Representation toXML() {
		// System.out.println("Request Original Ref " + getOriginalRef());
		// System.out.println("Request Entity " + getRequest().getEntityAsText()
		// +
		// " entity mediaType " + getRequest().getEntity().getMediaType());

		Reference ref = getRequest().getResourceRef();
		final String baseURL = ref.getHierarchicalPart();
		Form formQuery = ref.getQueryAsForm();

		try {
			DomRepresentation representation = new DomRepresentation(
					MediaType.TEXT_XML);

			Document d = representation.getDocument();
			Element elContacts = d.createElement(CONTACTS);
			d.appendChild(elContacts);

			Iterator<Contact> it = getSortedContacts(
					formQuery.getFirstValue(REQUEST_QUERY_SORT, LAST_NAME))
					.iterator();
			while (it.hasNext()) {
				Contact contact = it.next();

				Element el = d.createElement(CONTACT);

				Element id = d.createElement(ID);
				id.appendChild(d.createTextNode(String.format("%s",
						contact.getId())));
				el.appendChild(id);

				Element firstname = d.createElement(FIRST_NAME);
				firstname.appendChild(d.createTextNode(contact.getFirstName()));
				el.appendChild(firstname);

				Element lastname = d.createElement(LAST_NAME);
				lastname.appendChild(d.createTextNode(contact.getLastName()));
				el.appendChild(lastname);

				Element url = d.createElement(URL);
				url.appendChild(d.createTextNode(String.format("%s/%s",
						baseURL, contact.getId())));
				el.appendChild(url);

				elContacts.appendChild(el);
			}

			d.normalizeDocument();
			return representation;
		} catch (Exception e) {
			setStatus(Status.SERVER_ERROR_INTERNAL);
			return null;
		}
	}

	/**
	 * Handle HTTP GET method / Json
	 * 
	 * @return a JSON list of contacts.
	 */
	@Get("json")
	public Representation toJSON() {
		// System.out.println("Request Original Ref " + getOriginalRef());
		// System.out.println("Request Entity " + getRequest().getEntityAsText()
		// +
		// " entity mediaType " + getRequest().getEntity().getMediaType());

		try {
			JSONArray jcontacts = new JSONArray();
			Reference ref = getRequest().getResourceRef();
			final String baseURL = ref.getHierarchicalPart();
			Form formQuery = ref.getQueryAsForm();

			Iterator<Contact> it = getSortedContacts(
					formQuery.getFirstValue(REQUEST_QUERY_SORT, LAST_NAME))
					.iterator();

			while (it.hasNext()) {
				Contact contact = it.next();

				JSONObject jcontact = new JSONObject();
				jcontact.put(ID, String.format("%s", contact.getId()));
				jcontact.put(URL, String.format("%s/%s", baseURL, contact.getId()));
				jcontact.put(FIRST_NAME, contact.getFirstName());
				jcontact.put(LAST_NAME, contact.getLastName());
				jcontacts.put(jcontact);
			}

			JSONObject contacts = new JSONObject();
			contacts.put(CONTACTS, jcontacts);
			return new JsonRepresentation(contacts);
		} catch (Exception e) {
			setStatus(Status.SERVER_ERROR_INTERNAL);
			return null;
		}
	}

	/**
	 * Handle HTTP POST Method.
	 * 
	 * @param entity
	 *            contact to create.
	 * @return The new contact URI representation (Header Content-Location).
	 */
	@Post
	public Representation createContact(Representation entity) {
		Form form = new Form(entity);

		try {
			int id = Integer.parseInt(form.getFirstValue(ID, "-1"));
			if (id == -1) {
				System.out.println("id = -1");
				setStatus(Status.CLIENT_ERROR_EXPECTATION_FAILED,
						"Must provide 'id' field.");
				return null;
			}

			Contact contact = getContact(id) == null ? new Contact(id)
					: getContact(id);
			contact.setFirstName(form.getFirstValue(FIRST_NAME, "N/A"));
			contact.setLastName(form.getFirstValue(LAST_NAME, "N/A"));
			contact.setPhone(form.getFirstValue(PHONE, "N/A"));
			contact.setMail(form.getFirstValue(MAIL, "N/A"));

			Status createStatus = super.addContact(contact);
			setStatus(createStatus);

			Representation rep = new StringRepresentation("Contact created",
					MediaType.TEXT_PLAIN);
			rep.setLocationRef(String.format("%s/%s", getRequest()
					.getResourceRef().getIdentifier(), id));
			return rep;
		} catch (Exception e) {
			e.printStackTrace();
			setStatus(Status.SERVER_ERROR_INTERNAL);
			return null;
		}
	}

	/**
	 * Helper Method creating common form of ParameterInfo
	 * 
	 * @param name
	 *            ParameterInfo name
	 * @param description
	 *            ParameterInfo description
	 * @return ParameterInfo formatted : required=false, parameterStyle=Query,
	 *         defaultValue="N/A", type="String".
	 */
	private static ParameterInfo createCommonParameter(String name,
			String description) {
		return createCommonParameter(name, description, "N/A");
	}

	/**
	 * Helper Method creating common form of ParameterInfo
	 * 
	 * @param name
	 *            - ParameterInfo name
	 * @param description
	 *            - ParameterInfo description
	 * @param defaultValue
	 *            - ParameterInfo defaultValue.
	 * @return ParameterInfo formatted : required=false, parameterStyle=Query,
	 *         type="String".
	 */
	private static ParameterInfo createCommonParameter(String name,
			String description, String defaultValue) {
		ParameterInfo pi = new ParameterInfo(name, false, "String",
				ParameterStyle.QUERY, description);
		pi.setDefaultValue(defaultValue);
		return pi;

	}

	/**
	 * Helper method creating an OptionInfo
	 * 
	 * @param value
	 *            Option's value
	 * @param description
	 *            - Option's description
	 * @return an OptionInfo with the specified values.
	 */
	private static OptionInfo createOptionInfo(String value, String description) {
		OptionInfo oi = new OptionInfo(description);
		oi.setValue(value);
		return oi;
	}
}