struct keysym_to_ASCII{
  u32 X_keysym;
  u8 ASCII;
};

bool keysym_to_RTT(xcb_keysym_t keysym,u8 &ASCII);
