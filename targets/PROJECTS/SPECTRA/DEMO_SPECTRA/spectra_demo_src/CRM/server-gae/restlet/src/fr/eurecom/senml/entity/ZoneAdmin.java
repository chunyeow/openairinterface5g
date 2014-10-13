package fr.eurecom.senml.entity;

import java.util.Collections;
import java.util.List;
import java.util.SortedSet;
import java.util.logging.Logger;

import javax.jdo.annotations.Index;
import javax.jdo.annotations.Inheritance;
import javax.jdo.annotations.NotPersistent;
import javax.jdo.annotations.PersistenceCapable;
import javax.jdo.annotations.Persistent;

import com.google.appengine.api.datastore.Key;
import com.google.appengine.api.datastore.KeyFactory;

import fr.eurecom.senml.persistence.JDOStorage;
import fr.eurecom.senml.persistence.JDOStorageZone;

/**
 * Not thread-safe.
 * 
 * @author dalmassi
 * 
 */
@PersistenceCapable
@Inheritance(customStrategy = "complete-table")
// @Index(name="index_parentKey", members={"parentKeyString"})
public class ZoneAdmin extends Zone implements IZoneAdmin {
	@NotPersistent
	private static final Logger log = Logger.getLogger("Zone");

	@Persistent
	private boolean isActive = true;

	public ZoneAdmin(String name, ZoneAdmin parent, String type) {
		super(KeyFactory.createKey(ZoneAdmin.class.getSimpleName(), name),
				name, parent == null ? null : parent.getKey(), type);
		JDOStorage.getInstance().write(this);
	}

	@Override
	public void destroy() {
		isActive = false;
		parentKey = null;

		List<ISensorAdmin> sensors = JDOStorageZone.getInstance().getSensors(
				this);

		for (ISensorAdmin s : sensors) {
			s.destroy();
		}

		JDOStorage.getInstance().delete(this);
		// TODO: Possible Race condition here. The delete is not effective when
		// method JDOStorage delete returns.
		log.fine("Removed from database " + this.name);
	}

	@Override
	public boolean linkParentZone(String parentZoneKey) {
		if (isActive && checkKey(parentZoneKey)) {
			this.parentKey = parentZoneKey;
			JDOStorage.getInstance().write(this);
			return isActive;
		}
		return false;
	}

	@Override
	public boolean linkParentZone(IZone parentZone) {
		return parentZone == null ? false : linkParentZone(parentZone.getKey());
	}

	@Override
	public void unlinkParentZone() {
		if (parentKey != null) {
			parentKey = null;
			JDOStorage.getInstance().write(this);
		}
	}

	private boolean checkKey(String key) {
		return key != null && !key.trim().isEmpty();
	}
}
