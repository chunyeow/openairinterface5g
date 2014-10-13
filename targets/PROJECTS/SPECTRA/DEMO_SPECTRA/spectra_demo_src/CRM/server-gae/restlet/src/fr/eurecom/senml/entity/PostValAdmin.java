package fr.eurecom.senml.entity;

import java.util.UUID;

import javax.jdo.annotations.Persistent;

import fr.eurecom.senml.persistence.JDOStorage;

public class PostValAdmin implements IPostVal {
	
	@Persistent
	boolean active = true;

	public PostValType addPost(String name, String val, long timeMeasure) {
		PostValType m = new PostValType(UUID.randomUUID().toString(), name, val, timeMeasure);
		JDOStorage.getInstance().write(m);
		//JDOStorage.getInstance().deleteAll(PostValType.class);
		return m;
		//return null;
	}
}
