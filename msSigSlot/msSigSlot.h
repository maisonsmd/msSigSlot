#pragma once

enum class SlotType : uint8_t { Function, Method };

template <typename ...params_t>
class Slot
{
protected:
	SlotType _slotType;
	Slot(SlotType slotType) : _slotType(slotType) {}

public:
	virtual ~Slot() {}

	virtual void operator()(params_t... params) const = 0;

	virtual bool operator==(const Slot<params_t...> * slot) const = 0;

	virtual Slot<params_t...>* clone() const = 0;

	SlotType slotType() const {
		return _slotType;
	}
};

template <typename Signature>
class FunctionSlot;

template <typename return_t, typename ...params_t>
class FunctionSlot<return_t(params_t...)> final: public Slot<params_t...>
{
private:
	typedef return_t(*FuncPtr)(params_t...);
	FuncPtr _funcPtr;
public:
	FunctionSlot(FuncPtr f) : Slot<params_t...>(SlotType::Function), _funcPtr(f) {}

	Slot<params_t...> * clone() const {
		return new FunctionSlot<return_t(params_t...)>(_funcPtr);
	}

	void operator() (params_t... params) const {
		(_funcPtr)(params...);
	}

	bool operator==(const Slot<params_t...> * slot) const {
		if (slot && slot->slotType() == Slot<params_t... >::_slotType) {
			const FunctionSlot<return_t(params_t...)>* functSlot = reinterpret_cast<const FunctionSlot<return_t(params_t...)>*>(slot);
			return functSlot && functSlot->_funcPtr == this->_funcPtr;
		}
		return false;
	}
};

template <typename class_t, typename Signature>
class MethodSlot;

template <typename class_t, typename return_t, typename ...params_t>
class MethodSlot<class_t, return_t(params_t...)> final : public Slot<params_t...>
{
private:
	typedef return_t(class_t::*method_t)(params_t...);
	class_t * obj;
	method_t method;

public:
	MethodSlot(class_t *obj, method_t f) : Slot<params_t...>(SlotType::Method), obj(obj), method(f) {}

	Slot<params_t...> * clone() const {
		return new MethodSlot<class_t, return_t(params_t...)>(obj, method);
	}

	void operator() (params_t... params) const {
		return (obj->*method)(params...);
	}

	bool operator==(const Slot<params_t...>* slot) const {
		if (slot && slot->slotType() == Slot<params_t...>::_slotType) {
			const MethodSlot<class_t, return_t(params_t...)>* methSlot = reinterpret_cast<const MethodSlot<class_t, return_t(params_t...)>*>(slot);
			return methSlot && methSlot->obj == obj && methSlot->method == method;
		}
		return false;
	}
};

template <typename Signature, int8_t Slots = 1>
class Signal;

template <int8_t Slots, typename return_t, typename ...params_t> class Signal <return_t(params_t...), Slots>
{
private:
	Slot<params_t...> * _connections[Slots];
	int8_t _slotsCount = 0;

public:
	virtual ~Signal() {
		for (uint8_t i = 0; i < _slotsCount; i++)
			delete _connections[i];
	}

	Signal & attach(const Slot<params_t...>& slot) {
		if (_slotsCount < Slots)
			_connections[_slotsCount++] = slot.clone();
		//delete the oldest connection
		else {
			for (int8_t i = 0; i < _slotsCount - 1; i++) {
				//shift left 1
				_connections[i] = _connections[i + 1];
			}
			//update new connection
			_connections[_slotsCount - 1] = slot.clone();
		}
		return *this;
	}

	Signal & attach(return_t(*func)(params_t...)) {
		return attach(FunctionSlot< return_t(params_t...)>(func));
	}

	template <typename class_t>
	Signal & attach(class_t *obj, return_t(class_t::*method)(params_t...)) {
		return attach(MethodSlot<class_t, return_t(params_t...)>(obj, method));
	}

	Signal & detach(const Slot<params_t...>& slot) {
		for (int8_t i = _slotsCount - 1; i >= 0; i--)
			if (slot == _connections[i]) {
				delete _connections[i];

				for (int j = i; j < _slotsCount - 1; j++)
					_connections[j] = _connections[j + 1];

				_connections[_slotsCount] = NULL;

				_slotsCount--;
			}
		return *this;
	}

	Signal & detach(return_t(*func)(params_t...)) {
		return detach(FunctionSlot< return_t(params_t...)>(func));
	}

	template <typename class_t>
	Signal & detach(class_t *obj, return_t(class_t::*method)(params_t...)) {
		return detach(MethodSlot<class_t, return_t(params_t...)>(obj, method));
	}

	Signal & operator += (const Slot<params_t...>& slot) {
		return attach(slot);
	}

	Signal & operator += (return_t(*func)(params_t...)) {
		return attach(func);
	}

	Signal & operator -= (const Slot<params_t...>& slot) {
		return detach(slot);
	}

	Signal & operator -= (return_t(*func)(params_t...)) {
		return detach(func);
	}

	void fire(params_t...params) const {
		for (int i = 0; i < _slotsCount; i++)
			(*_connections[i])(params...);
	}

	void operator ()(params_t...params)const {
		fire(params...);
	}
};
