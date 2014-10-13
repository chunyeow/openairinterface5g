package fr.eurecom.senml.entity;

import java.util.List;
import java.util.logging.Level;
import java.util.logging.Logger;

import javax.jdo.annotations.NotPersistent;
import javax.jdo.annotations.PersistenceCapable;
import javax.jdo.annotations.Persistent;
import javax.jdo.annotations.PrimaryKey;

import com.google.appengine.api.datastore.Key;
import com.google.appengine.api.datastore.KeyFactory;

import fr.eurecom.senml.persistence.JDOStorageZone;

@PersistenceCapable(detachable="true")
public class Zone implements IZone {
	@NotPersistent
	private static final Logger log = Logger.getLogger("Zone");

	@Persistent
	@PrimaryKey
	protected Key key;
	
	@Persistent
	protected String name = "";
	
	@Persistent 
	protected String parentKey = null;
	
	@Persistent
	protected String zoneType = "";
	
/**
 * Instantiates a zone. 
 * @param key Zone key
 * @param name Zone name 
 * @param parentKey Zone parent key. Can be null. 
 * @param type 
 */
	protected Zone(Key key, String name, String parentKey, String type) {
		this.key = key; 
		this.name = name;
		this.parentKey = parentKey;
		this.zoneType = type;
	}

	@Override
	public List<IZone> getSubZones() {
		long before = System.currentTimeMillis();
		
		List<IZone> childs = JDOStorageZone.getInstance().getSubZones(this);
		
		long after = System.currentTimeMillis();
		log.info("getSubzone " + name + " taken ms:" + (after - before));
		return childs;
	}
	
	@Override
	public List<ISensor> getSensors() {
		long before = System.currentTimeMillis();
		
		List<ISensor> sensors = JDOStorageZone.getInstance().getSensors(this);
		
		long after = System.currentTimeMillis();
		log.info("getSensors " + name + " taken ms:" + (after - before));
		return sensors;
	}

	@Override
	public String getName() {
		return name;
	}
	
	@Override
	public String getKey() {
		return KeyFactory.keyToString(key);
	}
	
	@Override
	public String getType() {
		return zoneType;
	}

	@Override
	public int compareTo(IZone o) {
		return this.name.compareTo(o.getName());
	}

	@Override
	public IZone getParentZone() {
		Zone p = null;
		if (parentKey != null) {
			try {
				p = JDOStorageZone.getInstance().getById(parentKey, this.getClass());
			}
			catch (Exception e) {
				log.log(Level.WARNING, "Error retrieving parent zone", e);
			}
		}	
		return p;
	}
}
