package fr.eurecom.senml.entity;

import java.util.List;

public interface IZone extends Comparable<IZone> {
	
//	public boolean hasSensors();
	
//	public boolean hasSubZones();
	
	public List<IZone> getSubZones();

	public List<ISensor> getSensors();

	public String getName();
	
	public String getKey();
	
	public IZone getParentZone();

	String getType();
	
}
