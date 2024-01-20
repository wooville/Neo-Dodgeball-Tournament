#pragma once

struct ScriptedBehaviourComponent {
	std::shared_ptr <IScriptedBehaviour> script;

	ScriptedBehaviourComponent(const std::shared_ptr <IScriptedBehaviour>& script = nullptr){
		this->script = script;
	}
};