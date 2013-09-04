package io.brotherhood.usynergy.bean;

public class ServerEntity {
	public int id;
	public String ipadd;
	public String port;
	public int checked = 0;

	public String getFullAddress(){
		return ipadd + " : " + port;
	}

	public String toString() {
		return "id=" + id + "#ipadd=" + ipadd + "#prot=" + port + "#checked=" + checked;
	}
}
