package fr.eurecom.restlet.resources.gae;

import org.restlet.Request;
import org.restlet.Response;
import org.restlet.Restlet;
import org.restlet.ext.wadl.ApplicationInfo;
import org.restlet.ext.wadl.DocumentationInfo;
import org.restlet.ext.wadl.WadlApplication;
import org.restlet.resource.Directory;
import org.restlet.routing.Router;

import fr.eurecom.restlet.resources.common.ContactResource;
import fr.eurecom.restlet.resources.common.ContactsResource;
import fr.eurecom.restlet.resources.common.ContactsSubResource;

public class Dispatcher extends WadlApplication {

	@Override
	public Restlet createInboundRoot() {
		Router router = new Router(getContext());
		router.attachDefault(new Directory(getContext(), "war:///"));
// web xml mapping = /*		
//		router.attach("/contacts", ContactsResource.class);
//		router.attach("/contacts/{id}", ContactResource.class);
//		web xml mapping = /contacts/*		
		router.attach("", ContactsResource.class);
		router.attach("/subscribe", ContactsSubResource.class);
		router.attach("/{id}", ContactResource.class);
		return router;
	}

	@Override
	public ApplicationInfo getApplicationInfo(Request request, Response response) {
		ApplicationInfo info = super.getApplicationInfo(request, response);

		DocumentationInfo docInfo = new DocumentationInfo(
				"emulator-box-services application documentation");
		docInfo.setTitle("Emulator Box Services application infos.");
		info.setDocumentation(docInfo);

		return info;
	}
}
