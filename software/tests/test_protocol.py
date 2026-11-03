from tactilehand.protocol import parse_frame


def test_valid_frame():
    f = parse_frame("T,1200,0.0,10.5,20.0,30.0,100.0,0.0,0.0,5.5,40.0,310.2,5\n")
    assert f is not None
    assert f.t_ms == 1200
    assert f.position[1] == 10.5
    assert f.press[4] == 310.2
    assert f.in_contact(0) and not f.in_contact(1) and f.in_contact(2)


def test_wrong_field_count():
    assert parse_frame("T,1,2,3") is None


def test_not_a_frame():
    assert parse_frame("OK") is None
    assert parse_frame("# tendon-tactile-hand fw 0.1") is None


def test_garbled_numbers():
    assert parse_frame("T,12,a,b,c,d,e,f,g,h,i,j,0") is None
